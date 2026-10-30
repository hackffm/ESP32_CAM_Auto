#ifndef WIFI_NETWORK_H
#define WIFI_NETWORK_H

#include <stddef.h>
#include <stdint.h>
#include <WiFi.h>

constexpr uint32_t NETWORK_INITIAL_CONNECT_MS = 30UL * 1000UL;

struct NetworkStatus {
  IPAddress ip;
  char ssid[34];
  char bssid[34];
  int channel;
  int rssi;
};

struct NetworkScanResult {
  char ssid[34];
  char bssid[18];
  int rssi;
  int auth;
};

enum class NetworkIndicator {
  Connecting,
  FallbackAp,
  Connected
};

using NetworkIndicatorCallback = void (*)(NetworkIndicator indicator);

void networkBegin(const char *ssid, const char *password, const char *hostname,
                  NetworkIndicatorCallback indicatorCallback);
bool networkConnectStaBlocking();
void networkTask();
bool networkStaHealthy();
void networkStartFallbackAP();
// Requests an immediate retry only in the boot-selected STA mode. Credentials are fixed until reboot.
void networkRequestReconnect(const char *ssid, const char *password,
                             const char *hostname);
bool networkIsApMode();
const NetworkStatus &networkGetStatus();
bool networkSampleStatus();
int networkScanNetworks();
bool networkGetScanResult(int index, NetworkScanResult &result);
bool networkLinkWasLost();
void networkClearLinkLostFlag();
void networkShutdown();

#endif
