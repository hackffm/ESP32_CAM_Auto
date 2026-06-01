/*********
 
   The MIT License (MIT) (but poisoned by GPL in the Face Library)
   Hackerspace-FFM e.V. ESP32 CAM Robot with Web Interface
   2026-06-02 Lutz Lisseck

  Derived from this work: 
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-cam-projects-ebook/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

const char version_string[] = "V1.1 compiled on " __DATE__ " at " __TIME__;

#define MOTOR_1_PIN_1    14
#define MOTOR_1_PIN_2    15
#define MOTOR_2_PIN_1    13
#define MOTOR_2_PIN_2    12
#define SERVO_1_PIN       2
#define WHITE_LED_PIN     4
#define RED_LED_PIN      33

// Wifi credentials are in a MyCreds.h file that must reside in /<HOME>/.platformio/lib/MyCreds/MyCreds.h
// see attic/MyCreds.h for an example
#if defined __has_include
#  if __has_include (<MyCredsHackffm.h>)
#    include <MyCredsHackffm.h>  // Define WIFI_SSID and WIFI_PASSWORD here - put this file in /<HOME>/.platformio/lib/MyCreds/MyCredsHackffm.h
#  else
#    define WIFI_SSID ""
#    define WIFI_PASSWORD ""
#  endif
#else
#  define WIFI_SSID ""
#  define WIFI_PASSWORD ""
#endif

// Replace with your network credentials, will be overwritten by values stored in LittleFS if available
char roboter_name[34] = "cambot"; // only use a-z, 0-9 and - in the name 
char wifi_ssid[34] = WIFI_SSID;  // "REPLACE_WITH_YOUR_SSID";
char wifi_password[66] = WIFI_PASSWORD; // "REPLACE_WITH_YOUR_PASSWORD";

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_wifi.h"
#include <WiFiMulti.h>
#include <DNSServer.h> // For captive portal
#include <ESPmDNS.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems
#include "esp_http_server.h"
#include "driver/ledc.h" 
#include <ArduinoOTA.h>
#include "PwmThing.h"
#include "FS.h"
#include <LittleFS.h> 

#define ENABLE_OTA 
int OTA_Status = 0; // 0=not enabled, 1=enabled, 2=in progress
DNSServer dnsServer;
bool APMode = false;

IPAddress current_IP(192, 168, 4, 1);
char      current_ssid[34] = ""; 
int       current_rssi = -50;

PwmThing MotorLeft, MotorRight, Servo1, Servo2, WhiteLED, RedLED;

WiFiMulti wifiMulti;



// Store data used in PwmThing.begin for Motors and Servos here in an array of a structto be stored in LittleFS and to be used on startup
struct PwmThingConfig {
  char name[20]; int pinA; int pinB; PwmThing::ThingType thingType; bool inverted;
  int servoMin; int servoZero; int servoMax; /* Only for servos*/
};

// Create an array of PwmThingConfig for the motors and servos and populate it with default values
const int numPwmThings = 4;
PwmThingConfig pwmThingConfigs[numPwmThings] = {
  {"MotorLeft", MOTOR_1_PIN_1, MOTOR_1_PIN_2, PwmThing::halfBridgeIdleHigh, false, 768, 4760, 9544},
  {"MotorRight", MOTOR_2_PIN_1, MOTOR_2_PIN_2, PwmThing::halfBridgeIdleHigh, false, 768, 4760, 9544},
  {"Servo1", SERVO_1_PIN, -1, PwmThing::servoMotor, false, 768, 4760, 9544},
  {"Servo2", -1, -1, PwmThing::servoMotor, false, 768, 4760, 9544}
};

uint32_t lastMotorCommandTime = 0;

#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM_B
//#define CAMERA_MODEL_WROVER_KIT

#include "cam_pindefs.h"
#include "cam_streamhandler.h"
#include "indexhtml_intern.h"  // will be replaced if /data/index.html in LittleFS exists
#include "settingshtml_intern.h"  // will be replaced if /data/settings.html in LittleFS exists

#include "helper_functions.h"

int32_t  WhiteLedMaxOnTimeMs = 30000; // Prevent LED overheating
int      WhiteLedTimeoutThresholdValue = 44; // Count down MaxOnTimeMs above this threshold, limit LED to this threshold afterwards

struct CameraConfig {
  int rotation; // 0 = no rotation, 1 = 180° rotation
  int size; // 0 = 320x240 (QVGA), 1 = 640x480 (VGA), 2 = 800x600 (SVGA), 3 = 1024x768 (XGA), 4 = 1280x1024 (SXGA), 5 = 1600x1200 (UXGA)
  int fps; // Limit FPS, 0 = auto, 1...7 = 2,5,10,15,20,25,30 fps
  int quality; // 0 = auto, 1 very poor (63), 2 poor (42), 3 medium (30), 4 good (18), 5 very good (6)
} cameraConfig = {0, 1, 0, 0};
// current values go to quality and frame_limit_ms in cam_streamhandler.h, they are updated from there when changed 

fs::FS &filesystem = LittleFS;

void setCameraToConfig() {
  sensor_t * s = esp_camera_sensor_get();
  framesize_t size2framesize[] = {FRAMESIZE_QVGA, FRAMESIZE_VGA, FRAMESIZE_SVGA, 
    FRAMESIZE_XGA, FRAMESIZE_SXGA, FRAMESIZE_UXGA};
  if(cameraConfig.size < 0 || cameraConfig.size > 5) cameraConfig.size = 1; // default to VGA if out of bounds  
  s->set_framesize(s, size2framesize[cameraConfig.size]);
  //s->set_quality(s, cameraConfig.quality);

  int fps_values[] = {15, 2, 5, 10, 15, 20, 25, 30};
  if(cameraConfig.fps < 0 || cameraConfig.fps > 7) cameraConfig.fps = 0; // default to auto if out of bounds
  int fps_value = fps_values[cameraConfig.fps];
  frame_limit_ms = fps_value > 0 ? 1000 / fps_value : 0; // 0 means no limit

  int quality_values[] = {33, 63, 42, 30, 18, 6}; // Map 0...5 to quality values (0=auto, 1=very poor, ..., 5=very good)
  if(cameraConfig.quality < 0 || cameraConfig.quality > 5) cameraConfig.quality = 0; // default to auto if out of bounds
  quality = quality_values[cameraConfig.quality];

  camera_sensor_info_t *info = esp_camera_sensor_get_info(&s->id);

  if((info->model == CAMERA_OV3660)) {
    if(cameraConfig.rotation == 1) {
      s->set_hmirror(s, 0);
      s->set_vflip(s, 1);
    } else {
      s->set_hmirror(s, 1);
      s->set_vflip(s, 0);
    }
    //s->set_xclk(s, LEDC_TIMER_0, 8000000UL);
    s->set_pll(s, 0, 25, 1, 0, 0, 0, 1, 10); // pushes 8MHz ext to same internally as 20mhz before
    s->set_reg(s, 0x302c, 0xc0, 0x00); // Reduce pad driving strength for better EMI/radio
    s->set_reg(s, 0x6706, 0x0f, 0x03); // Adjust temperature sampling frequency to 8 MHz XVCLK
  } else if(s->id.PID == OV2640_PID) {
    if(cameraConfig.rotation == 1) {
      s->set_hmirror(s, 1);
      s->set_vflip(s, 1);
    } else {
      s->set_hmirror(s, 0);
      s->set_vflip(s, 0);
    }
    s->set_gainceiling(s, (gainceiling_t)2); // default is 2x, set to 1x to reduce noise
    s->set_reg(s, 0x111, 0xff, 0x80); // activae clock doubler to compensate 8 Mhz 
  }

}

void storeCameraConfig() {
  char buffer[50];
  snprintf(buffer, sizeof(buffer), "%d,%d,%d,%d\n", cameraConfig.rotation, cameraConfig.size, cameraConfig.fps, cameraConfig.quality);
  writeFile("/camera_config.txt", buffer);
}

void loadCameraConfig() {
  char buffer[50];
  if(filesystem.exists("/camera_config.txt")) {
    File file = filesystem.open("/camera_config.txt", "r");
    if(file) {
      size_t len = file.readBytes(buffer, sizeof(buffer)-1);
      buffer[len] = '\0';
      sscanf(buffer, "%d,%d,%d,%d", &cameraConfig.rotation, &cameraConfig.size, &cameraConfig.fps, &cameraConfig.quality);
      file.close();
    }
  }
}

void initPwmThings() {
    // Disable UART RX if pinA or pinB is using GPIO3
    if(pwmThingConfigs[0].pinA == 3 || pwmThingConfigs[0].pinB == 3 || 
       pwmThingConfigs[1].pinA == 3 || pwmThingConfigs[1].pinB == 3 || 
       pwmThingConfigs[2].pinA == 3 || pwmThingConfigs[2].pinB == 3 || 
       pwmThingConfigs[3].pinA == 3 || pwmThingConfigs[3].pinB == 3) {
      Serial.println("Disabling UART RX because GPIO3 is used for IO.");
      delay(100); Serial.end();
      Serial.begin(115200, SERIAL_8N1, -1); // Disable RX pin by setting it to -1
    }
    MotorLeft.begin(pwmThingConfigs[0].pinA, pwmThingConfigs[0].pinB, pwmThingConfigs[0].thingType, pwmThingConfigs[0].inverted, 
      pwmThingConfigs[0].servoMin, pwmThingConfigs[0].servoZero, pwmThingConfigs[0].servoMax);
    MotorRight.begin(pwmThingConfigs[1].pinA, pwmThingConfigs[1].pinB, pwmThingConfigs[1].thingType, pwmThingConfigs[1].inverted,
      pwmThingConfigs[1].servoMin, pwmThingConfigs[1].servoZero, pwmThingConfigs[1].servoMax);
    Servo1.begin(pwmThingConfigs[2].pinA, pwmThingConfigs[2].pinB, pwmThingConfigs[2].thingType, pwmThingConfigs[2].inverted,
      pwmThingConfigs[2].servoMin, pwmThingConfigs[2].servoZero, pwmThingConfigs[2].servoMax);
    Servo2.begin(pwmThingConfigs[3].pinA, pwmThingConfigs[3].pinB, pwmThingConfigs[3].thingType, pwmThingConfigs[3].inverted,
      pwmThingConfigs[3].servoMin, pwmThingConfigs[3].servoZero, pwmThingConfigs[3].servoMax);
}

void storePwmThingConfigs() {
  char filename[30];
  char buffer[256];
  for(int i=0; i<numPwmThings; i++) {
    snprintf(filename, sizeof(filename), "/%s.txt", pwmThingConfigs[i].name);
    snprintf(buffer, sizeof(buffer), "%d,%d,%d,%d,%d,%d,%d\n", 
      pwmThingConfigs[i].pinA, pwmThingConfigs[i].pinB, (int)pwmThingConfigs[i].thingType, (int)pwmThingConfigs[i].inverted,
      pwmThingConfigs[i].servoMin, pwmThingConfigs[i].servoZero, pwmThingConfigs[i].servoMax);
    writeFile(filename, buffer);
  }
}

void loadPwmThingConfigs() {
  char filename[30];
  char buffer[256];
  for(int i=0; i<numPwmThings; i++) {
    snprintf(filename, sizeof(filename), "/%s.txt", pwmThingConfigs[i].name);
    if(filesystem.exists(filename)) {
      File file = filesystem.open(filename, "r");
      if(file) {
        size_t len = file.readBytes(buffer, sizeof(buffer)-1);
        buffer[len] = '\0';
        sscanf(buffer, "%d,%d,%d,%d,%d,%d,%d", 
          &pwmThingConfigs[i].pinA, &pwmThingConfigs[i].pinB, (int*)&pwmThingConfigs[i].thingType, (int*)&pwmThingConfigs[i].inverted,
          &pwmThingConfigs[i].servoMin, &pwmThingConfigs[i].servoZero, &pwmThingConfigs[i].servoMax);
        file.close();
      }
    }
  }
}

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

static esp_err_t index_handler(httpd_req_t *req)
{
  Serial.printf("Handling request for %s\n", req->uri);

  char filename[32] = {0,};
  const char *pData = NULL;
  size_t dataLen = 0;
  const char req_type[32] = "text/html";

  if(strcmp(req->uri, "/") == 0 || strcmp(req->uri, "/index.html") == 0) {
    strcpy(filename, "/index.html");
    pData = INDEX_HTML;
    dataLen = strlen(INDEX_HTML);
  } else if (strcmp(req->uri, "/settings") == 0 || strcmp(req->uri, "/settings.html") == 0) {
    strcpy(filename, "/settings.html");
    pData = SETTINGS_HTML;
    dataLen = strlen(SETTINGS_HTML);
  } else if (strcmp(req->uri, "/favicon.ico") == 0) {
    httpd_resp_set_type(req, "image/x-icon");
    return httpd_resp_send(req, (const char *)favicon, sizeof(favicon));
  } 

  if(filesystem.exists(filename) == false) {
    Serial.printf("Serving internal %s\n", filename);
    httpd_resp_set_type(req, req_type);
    return httpd_resp_send(req, pData, dataLen);  
  } 
  Serial.printf("Serving external %s from LittleFS\n", filename);
  File file = filesystem.open(filename, "r");
  if (!file) {
      httpd_resp_send_404(req);
      Serial.printf("Failed to open file %s\n", filename);
      return ESP_FAIL;
  }
  httpd_resp_set_type(req, req_type);

  char chunk[1024];
  size_t read_bytes;
  while((read_bytes = file.readBytes(chunk, sizeof(chunk))) > 0) {
  //  Serial.printf("Read %d bytes from file\n", read_bytes);
    if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
      file.close();
      httpd_resp_sendstr_chunk(req, NULL); // end response
      return ESP_FAIL;
    }
  }
  file.close();
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

char infotext[256] = "";

static esp_err_t info_handler(httpd_req_t *req){
  static char info[2048];
  int info_len = 0;
  // sprintf(infotext, "BSSID: %s, Camera: %s", WiFi.BSSIDstr().c_str(), info->name);
  info_len = snprintf(info, sizeof(info), "%s, Cam-Temp: %d°C, Free heap: %u bytes, Free PSRAM: %u bytes, \r\n", 
     infotext, camera_temp, ESP.getFreeHeap(), ESP.getFreePsram());
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "SSID: %s, BSSID: %s, Channel: %d, IP: %s \r\n", 
     current_ssid, WiFi.BSSIDstr().c_str(), WiFi.channel(), current_IP.toString().c_str());
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "WiFi RSSI: %d dBm, FPS: %d, kBytes/s: %d, Command/s: %d\r\n", 
     WiFi.RSSI(), fps, bps/1024, cps);     
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "Version: %s\r\n", version_string);  
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "| Name=\"%s\", A=\"FPS-Limit (%d fps)\", "
     "B=\"Quality (%d)\", C=\"LED (Boost remaining: %ds)\", D=\"Servo1 (%d)\", E=\"Servo2 (%d)\"", 
     roboter_name, 1000/frame_limit_ms, quality, max(WhiteLedMaxOnTimeMs/1000,0L), Servo1.getDuty(), Servo2.getDuty());
  httpd_resp_set_type(req, "text/plain");
  return httpd_resp_send(req, info, strlen(info));
}

static esp_err_t cors_options_handler(httpd_req_t *req) {
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "*");
    httpd_resp_send(req, NULL, 0);    // 204 No Content
    return ESP_OK;
}

static esp_err_t cmd_handler(httpd_req_t *req){
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
  // Array of char strings "x", "y", "z" for example, to be used as query keys
  const char* query_keys[] = {"x", "y", "a", "b", "c", "d", "e", "f"};
  int num_keys = sizeof(query_keys) / sizeof(query_keys[0]);
  static int key_values[8] = {0,0,0,0,0,0,0,0}; // Array to hold the values of the query keys
  int res = -1;
  char strbuf[120] = {0,};
  char strbuf2[120] = {0,};
  cps_count++; // Count commands per second for performance monitoring

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if(!buf){
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      Serial.printf("Received query: %s\n", buf);
      // Loop through the expected query keys and extract their values
      for (int i = 0; i < num_keys; i++) {
        if (httpd_query_key_value(buf, query_keys[i], variable, sizeof(variable)) == ESP_OK) {
          key_values[i] = atoi(variable);
          res = 0;
          // Serial.printf("Key: %s, Value: %d\n", query_keys[i], key_values[i]);
        } 
      }
      // query ml and mr for direct motor control
      if((httpd_query_key_value(buf, "ml", strbuf, sizeof(strbuf)) == ESP_OK) && 
         (httpd_query_key_value(buf, "mr", strbuf2, sizeof(strbuf2)) == ESP_OK)) {
        int ml = atoi(strbuf);
        int mr = atoi(strbuf2);
        MotorLeft.set(ml);
        MotorRight.set(mr);
        lastMotorCommandTime = millis();
        free(buf);
        return httpd_resp_send(req, NULL, 0);
        //Serial.printf("Motor command received - Left: %d, Right: %d\n", ml, mr);
      }
      if((httpd_query_key_value(buf, "wifi_ssid", strbuf, sizeof(strbuf)) == ESP_OK) && 
         (httpd_query_key_value(buf, "wifi_password", strbuf2, sizeof(strbuf2)) == ESP_OK)) {
        bool noerrors = true;  
        uri_decode(strbuf, strbuf, sizeof(strbuf));
        uri_decode(strbuf2, strbuf2, sizeof(strbuf2));
        strlcpy(wifi_ssid, strbuf, sizeof(wifi_ssid));
        if(!writeFile("/wifi_ssid.txt", wifi_ssid)) noerrors = false;
        strlcpy(wifi_password, strbuf2, sizeof(wifi_password));
        if(!writeFile("/wifi_password.txt", wifi_password)) noerrors = false;
        Serial.printf("Updated WiFi SSID: %s, Password: %s\n", wifi_ssid, wifi_password);
        if(noerrors) {
          httpd_resp_sendstr(req, "WiFi credentials updated successfully. Reboot to activate.");
        } else {
          httpd_resp_sendstr(req, "Failed to update WiFi credentials.");
        }
      }
      if((httpd_query_key_value(buf, "roboter_name", strbuf, sizeof(strbuf)) == ESP_OK)) {
        bool noerrors = true;  
        uri_decode(strbuf, strbuf, sizeof(strbuf));
        strlcpy(roboter_name, strbuf, sizeof(roboter_name));
        if(!writeFile("/roboter_name.txt", roboter_name)) noerrors = false;
        Serial.printf("Updated Roboter Name: %s\n", roboter_name);
        if(noerrors) {
          httpd_resp_sendstr(req, "Roboter name updated successfully.");
        } else {
          httpd_resp_sendstr(req, "Failed to update roboter name.");
        }
      }
      if((httpd_query_key_value(buf, "pwmThingRead", strbuf, sizeof(strbuf)) == ESP_OK)) {
        Serial.printf("Read request for PwmThing index: %s\n", strbuf);
        int index = atoi(strbuf);
        if(index >= 0 && index < numPwmThings) {
          char response[256];
          snprintf(response, sizeof(response), "%s,%d,%d,%d,%d,%d,%d,%d", 
            pwmThingConfigs[index].name, pwmThingConfigs[index].pinA, pwmThingConfigs[index].pinB, (int)pwmThingConfigs[index].thingType, (int)pwmThingConfigs[index].inverted,
            pwmThingConfigs[index].servoMin, pwmThingConfigs[index].servoZero, pwmThingConfigs[index].servoMax);
          httpd_resp_set_type(req, "text/plain");
          httpd_resp_send(req, response, strlen(response));
          free(buf);
          return ESP_OK;
        } 
      }
      if((strcmp(buf, "pwmThingAvailablePins") == 0)) {
        Serial.println("Available pins request received");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, availablePins, strlen(availablePins));
        free(buf);
        return ESP_OK;
      }
      if((strcmp(buf, "cameraConfigRead") == 0)) {
        Serial.println("Camera config request received");
        httpd_resp_set_type(req, "text/plain");
        char response[256];
        snprintf(response, sizeof(response), "%d,%d,%d,%d", 
          cameraConfig.rotation, cameraConfig.size, cameraConfig.fps, cameraConfig.quality);
        httpd_resp_send(req, response, strlen(response));
        free(buf);
        return ESP_OK;
      }
      if((httpd_query_key_value(buf, "cameraConfigWrite", strbuf, sizeof(strbuf)) == ESP_OK)) {
        Serial.println("Camera config write request received");
        int values[4];
        if(sscanf(strbuf, "%d,%d,%d,%d", &values[0], &values[1], &values[2], &values[3]) == 4) {
          cameraConfig.rotation = values[0];
          cameraConfig.size = values[1];
          cameraConfig.fps = values[2];
          cameraConfig.quality = values[3];
          Serial.printf("Updated Camera Config: rotation=%d, size=%d, fps=%d, quality=%d\n", 
            cameraConfig.rotation, cameraConfig.size, cameraConfig.fps, cameraConfig.quality);
          setCameraToConfig();
          storeCameraConfig(); 
        }
        free(buf);
        return ESP_OK;
      }
      if((strcmp(buf, "wifi_scan") == 0)) {
        Serial.println("Scan Wifis...");
        int n = WiFi.scanNetworks(false, true);
        String json = "[";
        for (int i = 0; i < n; i++) {
          if (i > 0) json += ",";
          json += "{\"ssid\":\"" + WiFi.SSID(i) + "\"";
          json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
          json += ",\"rssi\":"   + String(WiFi.RSSI(i));
          json += ",\"auth\":"   + String((int)WiFi.encryptionType(i));
          json += "}";
        }
        json += "]";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json.c_str(), json.length());


        free(buf);
        return ESP_OK;
      }
      if((httpd_query_key_value(buf, "pwmThingWrite", strbuf, sizeof(strbuf)) == ESP_OK)) {
        Serial.printf("Write request for PwmThing index: %s\n", strbuf);
        int values[8]; char name[20]; int index = -1;
        if(sscanf(strbuf, "%d,%19[^,],%d,%d,%d,%d,%d,%d,%d", &index, name, &values[1], &values[2], &values[3], &values[4], &values[5], &values[6], &values[7]) == 9) {
          if((index >= 0 && index < numPwmThings) && (strlen(name) > 0) && isUsablePin(values[1]) 
            && isUsablePin(values[2]) && (values[3] >= 0) && (values[3] < PwmThing::thingTypeCount)) {
            //strlcpy(pwmThingConfigs[index].name, name, sizeof(pwmThingConfigs[index].name));
            pwmThingConfigs[index].pinA = values[1];
            pwmThingConfigs[index].pinB = values[2];
            pwmThingConfigs[index].thingType = (PwmThing::ThingType)values[3];
            pwmThingConfigs[index].inverted = (bool)values[4];
            pwmThingConfigs[index].servoMin = values[5];
            pwmThingConfigs[index].servoZero = values[6];
            pwmThingConfigs[index].servoMax = values[7];
            Serial.printf("Updated PwmThing index %d: Name=%s, pinA=%d, pinB=%d, thingType=%d, inverted=%d, servoMin=%d, servoZero=%d, servoMax=%d\n", 
              index, pwmThingConfigs[index].name, pwmThingConfigs[index].pinA, pwmThingConfigs[index].pinB, (int)pwmThingConfigs[index].thingType, (int)pwmThingConfigs[index].inverted,
              pwmThingConfigs[index].servoMin, pwmThingConfigs[index].servoZero, pwmThingConfigs[index].servoMax);
          }
          initPwmThings();  
          storePwmThingConfigs(); // Store default configs if not already stored
        }
        res = 0;
      }
      if((httpd_query_key_value(buf, "reconnect", strbuf, sizeof(strbuf)) == ESP_OK)) {
        Serial.println("WiFi reconnect requested");
        httpd_resp_sendstr(req, "ok reconnecting triggered...");
        WiFi.disconnect();
        WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);      
        WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);  
        //WiFi.reconnect();
        wifiMulti.run();
        free(buf);
        return ESP_OK;
      }
      if((httpd_query_key_value(buf, "restart", strbuf, sizeof(strbuf)) == ESP_OK)) {
        Serial.println("Restart requested");
        httpd_resp_sendstr(req, "ok restarting triggered...");
        delay(100);
        free(buf);
        ESP.restart();
        return ESP_OK;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  // 
  int x = key_values[0]; // -255 to 255
  int y = key_values[1]; // -255 to 255  
  MotorLeft.set(y + x);
  MotorRight.set(y - x); 

  Servo1.set(key_values[5]*2);
  Servo2.set(key_values[6]*2);
  lastMotorCommandTime = millis();

  float a = key_values[2]/127.0; // Convert to -1.0 to 1.0
  float b = key_values[3]/127.0; // Convert to -1.0 to 1.0
  float c = key_values[4]/127.0; // Convert to -1.0 to 1.0
  float d = key_values[5]/127.0; // Convert to -1.0 to 1.0
  float e = key_values[6]/127.0; // Convert to -1.0 to 1.0

  //frame_limit_ms = mapFloat(a, -1.0, 1.0, 250.0, 20.0); 

  int wval = constrain(c * 255, 0, 255);
  if(WhiteLedMaxOnTimeMs <= 0) {
    if(wval > WhiteLedTimeoutThresholdValue) wval = WhiteLedTimeoutThresholdValue; // Limit LED to threshold value to prevent overheating
  }
  WhiteLED.set(wval);

  /*
  quality = constrain(((1.0-b)/2.0)*63,4, 63);  // 0...63 lower=higher quality
  static int prev_quality = 10;
  if(quality != prev_quality) { 
    sensor_t * s = esp_camera_sensor_get();
    s->set_quality(s, quality); 
    prev_quality = quality; 
  }
  */
  
  if(res){
    return httpd_resp_send_500(req);
  }

  return httpd_resp_send(req, NULL, 0);
}

void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t settings_uri = {
    .uri       = "/settings",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t favicon_uri = {
    .uri       = "/favicon.ico",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t info_uri = {
    .uri       = "/info",
    .method    = HTTP_GET,
    .handler   = info_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t cmd_uri = {
    .uri       = "/action",
    .method    = HTTP_GET,
    .handler   = cmd_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t cors_options = {
    .uri       = "/stream",           // Gleicher URI
    .method    = HTTP_OPTIONS,        // OPTIONS-Methode!
    .handler   = cors_options_handler,
    .user_ctx  = NULL
  };
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &settings_uri);
    httpd_register_uri_handler(camera_httpd, &favicon_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
    httpd_register_uri_handler(camera_httpd, &info_uri);
  } else Serial.println("Failed to start camera HTTP server");
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
    httpd_register_uri_handler(stream_httpd, &cors_options); 
  } else Serial.println("Failed to start stream HTTP server");
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(115200);
  Serial.setDebugOutput(false);
  uint32_t psramSize = psramFound() ? ESP.getPsramSize() : 0;
  Serial.printf("Reset: PSRAM: %uMB, Flash: %uMB\n", psramSize / (1024 * 1024), ESP.getFlashChipSize() / (1024 * 1024));

  WhiteLED.begin(WHITE_LED_PIN, -1, PwmThing::pwmOutGamma, false);
  RedLED.begin(RED_LED_PIN, -1, PwmThing::pwmOutGamma, true);
  
  // Initialize filesystem
  if(!LittleFS.begin()) {
    Serial.println("filesystem Mount Failed");
    error_blink(3, 500);
  }
  listDir("/", 0); // List root directory for debugging

  // try to load user data from files
  readFile("/roboter_name.txt", roboter_name, sizeof(roboter_name));
  readFile("/wifi_ssid.txt", wifi_ssid, sizeof(wifi_ssid));
  readFile("/wifi_password.txt", wifi_password, sizeof(wifi_password));
  Serial.printf("Loaded config: Name=%s, SSID=%s, Password=%s\n", roboter_name, wifi_ssid, wifi_password);

  loadPwmThingConfigs();
  initPwmThings();  
  storePwmThingConfigs(); // Store default configs if not already stored

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 8000000;
  config.pixel_format = PIXFORMAT_JPEG;     
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 30;
  config.fb_count = 2;
  
  if(!psramFound()){
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 30;
    config.fb_count = 1;
  } else {
  //  heap_caps_malloc_extmem_enable(30000);
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    error_blink(2, 1000);
  }

  sensor_t * s = esp_camera_sensor_get();
  camera_sensor_info_t *info = esp_camera_sensor_get_info(&s->id);

  loadCameraConfig();
  setCameraToConfig();
  storeCameraConfig(); // Store default config if not already stored

  // Wi-Fi connection
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(roboter_name); 
  WiFi.mode(WIFI_STA);
  WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
  WiFi.setAutoReconnect(true);
  wifiMulti.addAP(wifi_ssid, wifi_password);
  WhiteLED.set(32); // LED on
  Serial.printf("Connecting to WiFi SSID: %s\n", wifi_ssid);

  if(wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi, opening AP mode.");
    RedLED.set(255); // Red LED on to indicate WiFi connection failure
    wifiMulti.APlistClean(); WiFi.disconnect(true, true); delay(1000); 
    WiFi.AP.begin();
    WiFi.AP.create(roboter_name);
    WiFi.AP.enableDhcpCaptivePortal();
    APMode = true;

    // by default DNSServer is started serving any "*" domain name. It will reply
    // AccessPoint's IP to all DNS request (this is required for Captive Portal detection)
    if (dnsServer.start()) {
      Serial.println("Started DNS server in captive portal-mode");
    } else {
      Serial.println("Err: Can't start DNS server!");
    }
    current_IP = WiFi.softAPIP();
  } else {
    current_IP = WiFi.localIP();
    strcpy(current_ssid, WiFi.SSID().c_str());
  }
  //esp_wifi_set_max_tx_power(40);

  WiFi.setSleep(false);
  Serial.printf("\nWiFi connected. Camera Stream Ready! Go to: http://%s or http://%s.local\n", current_IP.toString().c_str(), roboter_name);
  MDNS.begin(roboter_name);
  MDNS.addService("_http", "_tcp", 80);
  #ifdef ENABLE_OTA
    ArduinoOTA.onStart([]() {
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      OTA_Status = 2; // Set status to in progress
      Serial.print("Start updating ");
      WhiteLED.set(0); 
      RedLED.set(255); // Red LED on to indicate OTA in progress
      httpd_stop(camera_httpd);
      httpd_stop(stream_httpd);
      esp_camera_deinit();
      Serial.println("... ");
    });
    ArduinoOTA.setHostname(roboter_name);
    OTA_Status = 1;
    ArduinoOTA.begin();  
  #endif

  if(WiFi.SSID() != "HACKFFM.DE") WhiteLedTimeoutThresholdValue = 1044; // Low light limit only for Hackerspace...

  // Start streaming web server
  startCameraServer();
  
  //analogWrite(WHITE_LED_PIN, 254); // LED low
  Serial.printf("ledcClockSource %d\n", ledcGetClockSource());
  
  
  Serial.println("===== Camera sensor info =====");
  Serial.print("Model enum: ");
  Serial.println((int)info->model);

  Serial.print("Name: ");
  Serial.println(info->name);

  sprintf(infotext, "Camera: %s", info->name);

  WhiteLED.printInfo();
  WhiteLED.set(28);
  Serial.println("You can change the name, WiFi SSID, or password using serial commands.");
  Serial.println("Use 'name <newname>', 'ssid <newssid>' or 'password <newpassword>'.");

}

int WiFiScanState = 0; // 0 = idle, 1 = scanning, 2 = scan done

void startWiFiScan() {
  Serial.println("Scan start");
  // WiFi.scanNetworks will return immediately in Async Mode.
  WiFiScanState = 1;
  WiFi.scanNetworks(true);  // 'true' turns Async Mode ON
}

void printScannedNetworks(uint16_t networksFound) {
  if (networksFound == 0) {
    Serial.println("no networks found");
  } else {
    Serial.println("\nScan done");
    Serial.print(networksFound);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < networksFound; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      // print BSSID
      Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x", WiFi.BSSID(i)[0], WiFi.BSSID(i)[1], WiFi.BSSID(i)[2], 
        WiFi.BSSID(i)[3], WiFi.BSSID(i)[4], WiFi.BSSID(i)[5]);
      Serial.print(" | ");
      Serial.printf("%4" PRIi32, WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2" PRIi32, WiFi.channel(i));
      Serial.print(" | ");
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:            Serial.print("open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
        default:                        Serial.print("unknown");
      }
      Serial.println();
      delay(10);
    }
    Serial.println("");
    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();
  }
}

void processSerial() {
  const int bufferSize = 128;
  static char inputBuffer[bufferSize];
  static int bufferIndex = 0;
  char c;
  if(Serial.available()) {
    c = Serial.read(); Serial.print(c); // Echo back to serial monitor
    if(c == '\r') return; // Ignore carriage return
    if(c >= 32) {
      inputBuffer[bufferIndex] = c;
      if(bufferIndex < (bufferSize - 2)) { // Leave space for null terminator
        bufferIndex++;
      }
    }
    if((c == 8) && (bufferIndex > 0)) { // Handle backspace
      bufferIndex--;
    }
    if(c == '\n') {
      inputBuffer[bufferIndex] = '\0'; // Null-terminate the string
      Serial.printf("Received command: %s\n", inputBuffer);
      if(strncmp(inputBuffer, "name ", 5) == 0) {
        strlcpy(roboter_name, inputBuffer + 5, sizeof(roboter_name));
        writeFile("/roboter_name.txt", roboter_name);
        Serial.printf("Updated name to: %s\n", roboter_name);
      } else if(strncmp(inputBuffer, "ssid ", 5) == 0) {
        strlcpy(wifi_ssid, inputBuffer + 5, sizeof(wifi_ssid));
        writeFile("/wifi_ssid.txt", wifi_ssid);
        Serial.printf("Updated WiFi SSID to: %s\n", wifi_ssid);
      } else if(strncmp(inputBuffer, "password ", 9) == 0) {
        strlcpy(wifi_password, inputBuffer + 9, sizeof(wifi_password));
        writeFile("/wifi_password.txt", wifi_password);
        Serial.printf("Updated WiFi password to: %s\n", wifi_password);
      } else if(strncmp(inputBuffer, "scan", 4) == 0) {
        startWiFiScan();
      } else if(strncmp(inputBuffer, "reboot", 6) == 0) {
        Serial.println("Rebooting...");
        delay(100);
        ESP.restart();
      } else {
        Serial.println("Unknown command. Use 'name <newname>', 'ssid <newssid>' or 'password <newpassword>'.");
      }
      bufferIndex = 0; // Reset buffer index for next command
    } 
  }
}



void loop() {
  if(OTA_Status < 2) calc_fps();

  #ifdef ENABLE_OTA
  ArduinoOTA.handle(); // allow OTA updates
  #endif

  if(WhiteLED.get() > WhiteLedTimeoutThresholdValue) {
    if(WhiteLedMaxOnTimeMs > 0) {
      WhiteLedMaxOnTimeMs -= 10; // Count down timer
    } else {
      WhiteLED.set(WhiteLedTimeoutThresholdValue); // Limit LED to threshold value to prevent overheating
    }
  } else if(WhiteLedMaxOnTimeMs < 30000) {
    WhiteLedMaxOnTimeMs += 1; // Count up timer when LED is off or low
  }

  if(lastMotorCommandTime > 0 && (millis() - lastMotorCommandTime > 5000)) {
    MotorLeft.set(0);
    MotorRight.set(0);
    lastMotorCommandTime = 0;
    Serial.println("Motor command timeout, stopping motors");
  }

  processSerial(); // Check for serial commands 

  //if(!APMode) wifiMulti.run(); // Keep WiFi connection alive, will reconnect if connection is lost

  // check WiFi Scan Async process
  int16_t WiFiScanStatus = WiFi.scanComplete();
  if(WiFiScanState == 1) {
    if(WiFiScanStatus >= 0) {
      WiFiScanState = 2; // Scan done, process results in next loop iteration
      printScannedNetworks(WiFiScanStatus);
      WiFiScanState = 0; // Reset to idle after processing
    }
  }

  delay(10);
}

