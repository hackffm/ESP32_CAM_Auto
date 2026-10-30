#include "wifi_network.h"

#include <Arduino.h>
#include <DNSServer.h>
#include <esp_wifi.h>
#include <string.h>

namespace {
constexpr uint32_t STA_RETRY_DELAY_MS = 3000UL;
constexpr uint32_t INITIAL_REBEGIN_AFTER_MS = 8500UL;
constexpr uint32_t HEALTH_INTERVAL_MS = 5000UL;

DNSServer dnsServer;
bool apMode = false;
bool initialized = false;
bool staModeSelected = false;
bool pinnedAp = false;
bool retryPending = false;
uint32_t retryAtMs = 0;
uint32_t healthSampleAtMs = 0;
uint32_t connectStartedAtMs = 0;
uint32_t lastStatusSampleMs = 0;
int rssiSampleSum = 0;
int rssiSampleCount = 0;
volatile bool disconnectedEvent = false;
volatile bool retryAfterDisconnect = false;
volatile bool gotIpEvent = false;
volatile bool lostIpEvent = false;
volatile bool linkLostFlag = false;
volatile bool hadStaLink = false;
uint8_t pinnedBssid[6] = {};
uint8_t pinnedChannel = 0;
NetworkStatus status = {IPAddress(192, 168, 4, 1), "", "N/A", 0, -50};
NetworkIndicatorCallback indicatorCallback = nullptr;
char configuredSsid[34] = "";
char configuredPassword[66] = "";
char configuredHostname[34] = "";

void notifyIndicator(NetworkIndicator indicator) {
  if (indicatorCallback) indicatorCallback(indicator);
}

void wifiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info) {
  if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
    disconnectedEvent = true;
    if (hadStaLink) {
      linkLostFlag = true;
      retryAfterDisconnect = true;
    }
    hadStaLink = false;
    Serial.printf("WiFi disconnected, reason=%u\n",
                  info.wifi_sta_disconnected.reason);
  } else if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
    gotIpEvent = true;
    hadStaLink = true;
    Serial.printf("WiFi got IP: %s\n", WiFi.localIP().toString().c_str());
  } else if (event == ARDUINO_EVENT_WIFI_STA_LOST_IP) {
    lostIpEvent = true;
    if (hadStaLink) {
      linkLostFlag = true;
      retryAfterDisconnect = true;
    }
    hadStaLink = false;
    Serial.println("WiFi lost IP");
  }
}

void startStaAttempt() {
  if (!staModeSelected || apMode || !initialized) return;
  notifyIndicator(NetworkIndicator::Connecting);
  Serial.printf("Starting STA connection attempt for SSID %s\n", configuredSsid);
  if (pinnedAp) {
    WiFi.begin(configuredSsid, configuredPassword, pinnedChannel, pinnedBssid, true);
  } else {
    WiFi.begin(configuredSsid, configuredPassword);
  }
  connectStartedAtMs = millis();
  retryPending = false;
}

bool captureFirstAccessPoint() {
  wifi_ap_record_t apInfo = {};
  if (esp_wifi_sta_get_ap_info(&apInfo) != ESP_OK) return false;
  memcpy(pinnedBssid, apInfo.bssid, sizeof(pinnedBssid));
  pinnedChannel = apInfo.primary;
  pinnedAp = true;
  return true;
}

void refreshStaStatus() {
  status.ip = WiFi.localIP();
  strlcpy(status.ssid, configuredSsid, sizeof(status.ssid));
  wifi_ap_record_t apInfo = {};
  if (esp_wifi_sta_get_ap_info(&apInfo) == ESP_OK) {
    snprintf(status.bssid, sizeof(status.bssid),
             "%02X:%02X:%02X:%02X:%02X:%02X (ch %u)",
             apInfo.bssid[0], apInfo.bssid[1], apInfo.bssid[2],
             apInfo.bssid[3], apInfo.bssid[4], apInfo.bssid[5],
             apInfo.primary);
    status.channel = apInfo.primary;
    status.rssi = apInfo.rssi;
  }
}
}  // namespace

void networkBegin(const char *ssid, const char *password, const char *hostname,
                  NetworkIndicatorCallback callback) {
  strlcpy(configuredSsid, ssid ? ssid : "", sizeof(configuredSsid));
  strlcpy(configuredPassword, password ? password : "", sizeof(configuredPassword));
  strlcpy(configuredHostname, hostname ? hostname : "robot", sizeof(configuredHostname));
  indicatorCallback = callback;
  apMode = false;
  staModeSelected = true;
  initialized = true;
  pinnedAp = false;
  hadStaLink = false;
  retryPending = false;
  disconnectedEvent = gotIpEvent = lostIpEvent = retryAfterDisconnect = false;

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(configuredHostname);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(false);  // This module exclusively owns all retries.
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  esp_wifi_set_country_code("DE", true);
  WiFi.onEvent(wifiEventHandler);
  WiFi.disconnect(false, false);
  delay(50);
  startStaAttempt();
}

bool networkStaHealthy() {
  if (!staModeSelected || apMode) return false;
  const bool connected = WiFi.status() == WL_CONNECTED &&
                         WiFi.localIP() != IPAddress(0, 0, 0, 0);
  if (connected) {
    if (!pinnedAp) captureFirstAccessPoint();
    refreshStaStatus();
  }
  return connected;
}

bool networkConnectStaBlocking() {
  if (!initialized || !staModeSelected || apMode) return false;
  const uint32_t started = millis();
  while ((uint32_t)(millis() - started) < NETWORK_INITIAL_CONNECT_MS) {
    networkTask();
    if (networkStaHealthy()) {
      refreshStaStatus();
      notifyIndicator(NetworkIndicator::Connected);
      return true;
    }
    delay(20);
  }
  return networkStaHealthy();
}

void networkTask() {
  const uint32_t now = millis();
  if (apMode) {
    dnsServer.processNextRequest();
    return;
  }
  if (!staModeSelected) return;

  if (gotIpEvent) {
    gotIpEvent = false;
    disconnectedEvent = false;
    lostIpEvent = false;
    if (!pinnedAp) captureFirstAccessPoint();
    refreshStaStatus();
    retryPending = false;
    notifyIndicator(NetworkIndicator::Connected);
  }
  if (lostIpEvent) lostIpEvent = false;
  if (disconnectedEvent) {
    disconnectedEvent = false;
    notifyIndicator(NetworkIndicator::Connecting);
  }
  if (retryAfterDisconnect) {
    retryAfterDisconnect = false;
    retryAtMs = now + STA_RETRY_DELAY_MS;
    retryPending = true;
  }

  if (networkStaHealthy()) {
    healthSampleAtMs = now;
    retryPending = false;
  } else {
    if (hadStaLink) {
      linkLostFlag = true;
      hadStaLink = false;
      retryAfterDisconnect = true;
    }
  }

  if (!networkStaHealthy() &&
      (uint32_t)(now - connectStartedAtMs) >= INITIAL_REBEGIN_AFTER_MS &&
             !retryPending) {
    Serial.println("STA association timed out; restarting connection attempt");
    WiFi.disconnect(false, false);
    delay(100);
    startStaAttempt();
    healthSampleAtMs = now;
  } else if ((uint32_t)(now - healthSampleAtMs) >= HEALTH_INTERVAL_MS) {
    healthSampleAtMs = now;
    if (!retryPending) {
      retryAtMs = now + STA_RETRY_DELAY_MS;
      retryPending = true;
      notifyIndicator(NetworkIndicator::Connecting);
    }
  }

  if (retryPending && (int32_t)(now - retryAtMs) >= 0) {
    WiFi.disconnect(false, false);
    startStaAttempt();
  }
}

void networkStartFallbackAP() {
  if (!initialized || !staModeSelected || apMode) return;
  // Boot decision is final: fallback uses AP-only mode and never retries STA.
  WiFi.disconnect(false, false);
  staModeSelected = false;
  apMode = true;
  retryPending = false;
  WiFi.setAutoReconnect(false);
  WiFi.mode(WIFI_AP);
  WiFi.AP.begin();
  const bool started = WiFi.AP.create(configuredHostname);
  WiFi.AP.enableDhcpCaptivePortal();
  status.ip = WiFi.softAPIP();
  strlcpy(status.ssid, configuredHostname, sizeof(status.ssid));
  snprintf(status.bssid, sizeof(status.bssid), "N/A");
  status.channel = 1;
  notifyIndicator(NetworkIndicator::FallbackAp);
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", WiFi.softAPIP());
  Serial.printf("Fallback open AP %s at %s (%s)\n", configuredHostname,
                status.ip.toString().c_str(), started ? "started" : "start failed");
}

void networkRequestReconnect(const char *ssid, const char *password,
                             const char *hostname) {
  (void)ssid;
  (void)password;
  (void)hostname;
  if (staModeSelected && !apMode) {
    WiFi.disconnect(false, false);
    startStaAttempt();
  }
}

bool networkIsApMode() { return apMode; }
const NetworkStatus &networkGetStatus() { return status; }
// Consumers may use this sticky flag to decide whether live streams need recovery.
bool networkLinkWasLost() { return linkLostFlag; }
void networkClearLinkLostFlag() { linkLostFlag = false; }

bool networkSampleStatus() {
  const uint32_t now = millis();
  if ((uint32_t)(now - lastStatusSampleMs) < 100) return false;
  lastStatusSampleMs = now;
  int rssi = WiFi.RSSI();
  if (apMode) {
    wifi_sta_list_t stationList = {};
    if (esp_wifi_ap_get_sta_list(&stationList) == ESP_OK && stationList.num > 0)
      rssi = stationList.sta[0].rssi;
  } else if (networkStaHealthy()) {
    refreshStaStatus();
  }
  rssiSampleSum += rssi;
  ++rssiSampleCount;
  if (rssiSampleCount < 10) return false;
  status.rssi = rssiSampleSum / rssiSampleCount;
  rssiSampleSum = 0;
  rssiSampleCount = 0;
  return true;
}

int networkScanNetworks() { return WiFi.scanNetworks(false, true); }

bool networkGetScanResult(int index, NetworkScanResult &result) {
  if (index < 0 || index >= WiFi.scanComplete()) return false;
  const String ssid = WiFi.SSID(index);
  const String bssid = WiFi.BSSIDstr(index);
  strlcpy(result.ssid, ssid.c_str(), sizeof(result.ssid));
  strlcpy(result.bssid, bssid.c_str(), sizeof(result.bssid));
  result.rssi = WiFi.RSSI(index);
  result.auth = (int)WiFi.encryptionType(index);
  return true;
}

void networkShutdown() {
  dnsServer.stop();
  WiFi.disconnect(true, false);
  WiFi.mode(WIFI_OFF);
  esp_wifi_deinit();
}
