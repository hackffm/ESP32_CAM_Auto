/*********
 
   The MIT License (MIT) 
   Hackerspace-FFM e.V. ESP32 CAM Robot with Web Interface
   https://www.hackerspace-ffm.de/wiki/index.php?title=FPV-Roboter
   2026-06-12 Lutz Lisseck 

  Derived from this work: 
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-cam-projects-ebook/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

const char version_string[] = "V1.14";

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
#  if __has_include (<MyCredsLabyrinth.h>)
#    include <MyCredsLabyrinth.h>  // Define WIFI_SSID and WIFI_PASSWORD here - put this file in /<HOME>/.platformio/lib/MyCreds/MyCredsHackffm.h
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

// =============================================================
//  User-configurable channel list for scanning.
//  Fill this with the channels you actually want to probe.
//  Using non-overlapping channels (e.g. 1, 6, 11 or 1, 7, 13)
//  keeps the scan short and the stream interruption minimal.
//  Leave empty {} to scan ALL channels (slower).
// =============================================================
static const uint8_t ROAM_SCAN_CHANNELS[] = {}; //{1,2,3,4,5,6,7,8,9,10,11,12,13}; // { 1, 7, 13 };
static const size_t  ROAM_SCAN_CHANNEL_COUNT =
        sizeof(ROAM_SCAN_CHANNELS) / sizeof(ROAM_SCAN_CHANNELS[0]);

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_wifi.h"
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// =============================================================
//  Detect whether 802.11r Fast Transition is compiled into
//  this build. This is decided at compile time.
// =============================================================
#ifdef CONFIG_ESP_WIFI_11R_SUPPORT
  #define ROAM_FT_AVAILABLE 1
#else
  #define ROAM_FT_AVAILABLE 0
#endif

#define ENABLE_OTA 
int OTA_Status = 0; // 0=not enabled, 1=enabled, 2=in progress
DNSServer dnsServer;
bool APMode = false;
bool RoamForceTriggered = false;


IPAddress current_IP(192, 168, 4, 1);
char      current_ssid[34] = ""; 
char      current_bssid[34] = ""; 
int       current_channel = 0;
int       current_rssi = -50;

PwmThing MotorLeft, MotorRight, Servo1, Servo2, WhiteLED, RedLED;

WiFiMulti wifiMulti;

// Gamemaster stuff
const unsigned int gmPort = 8071;        // UDP port for gamemaster
IPAddress gmLastRemoteIp;
uint16_t  gmLastRemotePort = 0;
bool      gmLastRemoteValid = false;
WiFiUDP gmUdp;

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
uint32_t powerDownTimer = 0;
uint32_t powerDownTimeout = 60 * 60000UL; // Power down 30 min

struct LightVariables {
  // All values are usually 0...255 for 0% to 100% brightness
  int requestedValue = -1;          // User request goes here, -1 means no request, applied in main loop with limits and timeouts
  int Value = 0;                    // What the light is currently set to, timeouts and limits are applied to this value, and this is what is actually set on the LED
  int lowValue = 50;                // User defined value for candle/torch light (no time limit if below limitLowValue)
  int highValue = 255;              // User defined value for strong brightness (will be time limited)
  int limitLowValue = 75;           // Unlimited brightness up to this value, above this value, the timeout will apply
  int limitHighValue = 250;         // Absolute maximum allowed brightness, even for boost, to prevent overheating
  int boostTime = 30000;            // Available boost time in milliseconds
  int boostTimeMax = 30000;         // Maximum boost time in milliseconds (e.g. to calculate remaining boost time percentage)
} LightVars;

struct ServoVariables {
  // All values -255 ... 255 typically (percentage of PwmThing ServoMin and ServoMax)
  int requestedValue = -9999;    // User request goes here, -9999 means no request, applied in main loop with limits and timeouts
  int Value = 0;                 // What the servo is currently set to
  int lowValue = -30;            // User defined value for one lower position 
  int highValue = 150;           // User defined value for one higher position
} Servo1Vars, Servo2Vars;

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

struct CameraConfig {
  int rotation; // 0 = no rotation, 1 = 180° rotation
  int size; // 0 = 320x240 (QVGA), 1 = 400x296 (CIF), 2 = 640x480 (VGA), 3 = 800x600 (SVGA), 4 = 1024x768 (XGA), 5 = 1280x1024 (SXGA), 6 = 1600x1200 (UXGA)
  int fps; // Limit FPS, 0 = auto, 1...7 = 2,5,10,15,20,25,30 fps
  int quality; // 0 = auto, 1 very poor (63), 2 poor (42), 3 medium (30), 4 good (18), 5 very good (6)
} cameraConfig = {0, 2, 0, 0};
// current values go to quality and frame_limit_ms in cam_streamhandler.h, they are updated from there when changed 

fs::FS &filesystem = LittleFS;

void setCameraToConfig() {
  sensor_t * s = esp_camera_sensor_get();
  framesize_t size2framesize[] = {FRAMESIZE_QVGA, FRAMESIZE_CIF, FRAMESIZE_VGA, FRAMESIZE_SVGA, 
    FRAMESIZE_XGA, FRAMESIZE_SXGA, FRAMESIZE_UXGA};
  if(cameraConfig.size < 0 || cameraConfig.size > 6) cameraConfig.size = 2; // default to VGA if out of bounds  
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
    // s->set_brightness(s, 0); // Increase brightness to reduce noise in dark areas
    //s->set_raw_gma(s, 1); s->set_reg(s, 0x5480, 1, 0);
    //s->set_gainceiling(s, GAINCEILING_32X);
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

void loadLightServoLowHighVals() {
  char buffer[50];
  if(filesystem.exists("/ls_lh_vals.txt")) {
    File file = filesystem.open("/ls_lh_vals.txt", "r");
    if(file) {
      size_t len = file.readBytes(buffer, sizeof(buffer)-1);
      buffer[len] = '\0';
      sscanf(buffer, "%d,%d,%d,%d,%d,%d", &LightVars.lowValue, &LightVars.highValue, &Servo1Vars.lowValue, &Servo1Vars.highValue, &Servo2Vars.lowValue, &Servo2Vars.highValue);
      file.close();
    }
  }
}

void storeLightServoLowHighVals() {
  char buffer[50];
  snprintf(buffer, sizeof(buffer), "%d,%d,%d,%d,%d,%d\n", LightVars.lowValue, LightVars.highValue, Servo1Vars.lowValue, Servo1Vars.highValue, Servo2Vars.lowValue, Servo2Vars.highValue);
  writeFile("/ls_lh_vals.txt", buffer);
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
  info_len = snprintf(info, sizeof(info), "%s, Cam-Temp: %d°C, Free heap: %u, Free PSRAM: %u, \r\n", 
     infotext, camera_temp, ESP.getFreeHeap(), ESP.getFreePsram());
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "SSID: %s, BSSID: %s, IP: %s \r\n", 
     current_ssid, current_bssid,  current_IP.toString().c_str());
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "Avg RSSI: %d dBm, FPS: %d/%d, Quality: %d, kBytes/s: %d, Command/s: %d\r\n", 
     current_rssi, fps, frame_limit_ms?1000/frame_limit_ms:0, quality, bps/1024, cps);    
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "ML: %d, MR: %d, Powerdown timer: %lu s\r\n", 
     MotorLeft.get(), MotorRight.get(), (powerDownTimeout - (millis() - powerDownTimer))/1000);      
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "Version: %s %s, Uptime: %lu s\r\n", version_string, CompileTime, millis() / 1000);  
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "| Name=\"%s\", A=\"FPS-Limit (%d fps)\", "
     "B=\"Quality (%d)\", C=\"LED (Boost remaining: %ds)\", D=\"Servo1 (%d)\", E=\"Servo2 (%d)\", ", 
     roboter_name, frame_limit_ms?1000/frame_limit_ms:0, quality, LightVars.boostTime/1000,0, Servo1.getDuty(), Servo2.getDuty());
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "lightValue=\"%d\", lightLowValue=\"%d\", lightHighValue=\"%d\", "
      "lightLimitLowValue=\"%d\", lightLimitHighValue=\"%d\", lightBoostTime=\"%d\", lightBoostTimeMax=\"%d\", ", 
     LightVars.Value, LightVars.lowValue, LightVars.highValue, LightVars.limitLowValue, LightVars.limitHighValue, 
     LightVars.boostTime/1000, LightVars.boostTimeMax/1000);   
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "Servo1Value=\"%d\", Servo1LowValue=\"%d\", Servo1HighValue=\"%d\", " 
      "Servo1RawValue=\"%d\", Servo2Value=\"%d\", Servo2LowValue=\"%d\", Servo2HighValue=\"%d\", Servo2RawValue=\"%d\", ",
     Servo1Vars.Value, Servo1Vars.lowValue, Servo1Vars.highValue, Servo1.getDuty(), 
     Servo2Vars.Value, Servo2Vars.lowValue, Servo2Vars.highValue, Servo2.getDuty());
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "Version=\"%s\", ", version_string);

  char status[255] = "FPS: 12/30, Q:32, RSSI:-75";
  snprintf(status, sizeof(status), "FPS:%d/%d, Q:%d, RSSI:%d, Ch:%d", fps, frame_limit_ms?1000/frame_limit_ms:0, 
     quality, current_rssi, current_channel);
  info_len += snprintf(info + info_len, sizeof(info) - info_len, "Status=\"%s\", ", status);
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

// HTTP Error (404) Handler - Redirects all requests to the root page
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err) {
    httpd_resp_set_status(req, "303 See Other"); // Set status
    httpd_resp_set_hdr(req, "Location", "/"); // Redirect to the "/" root directory
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
    ESP_LOGI(TAG, "Redirecting to root");
    return ESP_OK;
}

static esp_err_t cmd_handler(httpd_req_t *req){
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
  // Array of char strings "x", "y", "z" for example, to be used as query keys
  const char* query_keys[] = {"x", "y", "a", "b", "c", "d", "e", "f", "ml", "mr", "light", "servo1", "servo2", "lightLowValue", "lightHighValue", "Servo1LowValue", "Servo1HighValue", "Servo2LowValue", "Servo2HighValue"};
  int num_keys = sizeof(query_keys) / sizeof(query_keys[0]);
  static int key_values[19] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Array to hold the values of the query keys
  int key_values_changed[19] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Array to track which keys have changed for performance optimizations
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
          key_values_changed[i] = 1; // Mark this key as changed
          res = 0;
          // Serial.printf("Key: %s, Value: %d\n", query_keys[i], key_values[i]);
        } 
      }
      if(res == 0) {
        if(key_values_changed[8] || key_values_changed[9]) { // If x or y changed, update motors
          int ml = key_values[8]; // x + y for left motor
          int mr = key_values[9]; // x - y for right motor
          MotorLeft.set(ml);
          MotorRight.set(mr);
          lastMotorCommandTime = millis();
        }
        if(key_values_changed[0] || key_values_changed[1]) { 
          int x = key_values[0]; // -255 to 255
          int y = key_values[1]; // -255 to 255  
          MotorLeft.set(y + x);
          MotorRight.set(y - x); 
          lastMotorCommandTime = millis();
        }
        // Old sliders...
        if(key_values_changed[5] || key_values_changed[6]) {
          Servo1.set(key_values[5]*2);
          Servo2.set(key_values[6]*2);
          lastMotorCommandTime = millis();
        }
        if(key_values_changed[2] || key_values_changed[3] || key_values_changed[4]) {

          float a = key_values[2]/127.0; // Convert to -1.0 to 1.0
          float b = key_values[3]/127.0; // Convert to -1.0 to 1.0
          float c = key_values[4]/127.0; // Convert to -1.0 to 1.0

          LightVars.requestedValue = constrain(c * 255, 0, 255);
        }
        // New direct controls for servos and light
        if(key_values_changed[10]) { // light
          LightVars.requestedValue = constrain(key_values[10], 0, 255);
        }
        if(key_values_changed[11]) { // servo1
          //Servo1.set(key_values[11]);
          Servo1.startAnimation(2, 1000, Servo1.get(), key_values[11]);
        }
        if(key_values_changed[12]) { // servo2
          Servo2.set(key_values[12]);
        }
        if(key_values_changed[13] || key_values_changed[14] || key_values_changed[15] || key_values_changed[16] || key_values_changed[17] || key_values_changed[18]) { 
          if(key_values_changed[13]) LightVars.lowValue = constrain(key_values[13], 0, 255);
          if(key_values_changed[14]) LightVars.highValue = constrain(key_values[14], 0, 255);
          if(key_values_changed[15]) Servo1Vars.lowValue = constrain(key_values[15], -255, 255);
          if(key_values_changed[16]) Servo1Vars.highValue = constrain(key_values[16], -255, 255);
          if(key_values_changed[17]) Servo2Vars.lowValue = constrain(key_values[17], -255, 255);
          if(key_values_changed[18]) Servo2Vars.highValue = constrain(key_values[18], -255, 255);
          storeLightServoLowHighVals();
        }
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
      if((httpd_query_key_value(buf, "configDataRead", strbuf, sizeof(strbuf)) == ESP_OK)) {
        Serial.printf("Read request for configData index: %s\n", strbuf);
        int index = atoi(strbuf);
        if(index >= 0 && index < 9) {
          char response[256]; response[0] = '\0';
          char filename[30];
          snprintf(filename, sizeof(filename), "/configData_%d.txt", index);
          readFile(filename, response, sizeof(response));
          httpd_resp_set_type(req, "text/plain");
          httpd_resp_send(req, response, strlen(response));
          free(buf);
          return ESP_OK;
        } 
      }
      if((httpd_query_key_value(buf, "configDataWrite", strbuf, sizeof(strbuf)) == ESP_OK)) {
        // Extract index and remaining string from strbuf, where strbuf is expected to start 
        //with the index followed by a comma and then any amount of data.
        int index = -1;
        char *commaPos = strchr(strbuf, ',');
        if(commaPos != NULL) {
          *commaPos = '\0'; // Split the string at the comma
          index = atoi(strbuf); // Convert the part before the comma to an integer index
          char *valuePart = commaPos + 1; // The part after the comma is the value
          if(index >= 0 && index < 9) {
            char filename[30];
            snprintf(filename, sizeof(filename), "/configData_%d.txt", index);
            writeFile(filename, valuePart);
            Serial.printf("Updated configData index %d: %s\n", index, valuePart);
            httpd_resp_sendstr(req, "Config data updated successfully.");
          } else {
            httpd_resp_sendstr(req, "Invalid config data index.");
          }
        } else {
          httpd_resp_sendstr(req, "Invalid config data format.");
        }
        free(buf);
        return ESP_OK;
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
      if((strcmp(buf, "powerDownRead") == 0)) {
        Serial.println("Power down read request received");
        httpd_resp_set_type(req, "text/plain");
        char response[25];
        snprintf(response, sizeof(response), "%ld", powerDownTimeout/(1000UL*60));
        httpd_resp_send(req, response, strlen(response));
        free(buf);
        return ESP_OK;
      }
      if((httpd_query_key_value(buf, "powerDownWrite", strbuf, sizeof(strbuf)) == ESP_OK)) {
        int pdval = atoi(strbuf);
        Serial.printf("Write request for power down value: %d\n", pdval);
        powerDownTimeout = constrain(pdval,0,240) * 1000UL * 60;
        char response[25];
        snprintf(response, sizeof(response), "%ld", powerDownTimeout);
        writeFile("/powerdowntimeout.txt", response);
        powerDownTimer = millis(); 
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
      if((strcmp(buf, "shutdown") == 0)) {
        Serial.println("Shutdown requested");
        httpd_resp_sendstr(req, "ok shutdown triggered...");
        delay(100);
        free(buf);
        powerDownTimeout = 0xfffffffful;
        return ESP_OK;
      }
      if((strcmp(buf, "trig") == 0)) {
        Serial.println("Trigger requested");
        httpd_resp_sendstr(req, "Triggered...");
        delay(100);
        free(buf);
        RoamForceTriggered = true;
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
    .uri       = "/stream",           // same URI
    .method    = HTTP_OPTIONS,        // OPTIONS method!
    .handler   = cors_options_handler,
    .user_ctx  = NULL
  };
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &settings_uri);
    httpd_register_uri_handler(camera_httpd, &favicon_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
    httpd_register_uri_handler(camera_httpd, &info_uri);
    httpd_register_err_handler(camera_httpd, HTTPD_404_NOT_FOUND, http_404_error_handler);
  } else Serial.println("Failed to start camera HTTP server");
  config.server_port += 1; // Need extra port and server otherwise streaming blocks control commands
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
    httpd_register_uri_handler(stream_httpd, &cors_options); 
  } else Serial.println("Failed to start stream HTTP server");
  CameraRunning = true;
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  getCompileTime();
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  uint32_t psramSize = psramFound() ? ESP.getPsramSize() : 0;
  Serial.printf("Reset: PSRAM: %uMB, Flash: %uMB\n", psramSize / (1024 * 1024), ESP.getFlashChipSize() / (1024 * 1024));

    #ifdef CONFIG_ESP_WIFI_11R_SUPPORT
        Serial.println("802.11r (FT) is compiled in");
    #else
        Serial.println("802.11r NOT compiled in - falling back to fast reconnect");
    #endif

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
  char buf[32];
  readFile("/powerdowntimeout.txt", buf, sizeof(buf));
  sscanf(buf, "%ld", &powerDownTimeout);
  if(powerDownTimeout !=0 ) powerDownTimeout = constrain(powerDownTimeout, 120*1000UL, 240*60*1000UL); 

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
  
  if(!psramFound()){
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.fb_count = 1;
  } else {
  //  heap_caps_malloc_extmem_enable(30000);
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
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

  loadLightServoLowHighVals();

  // Wi-Fi connection
  WiFi.persistent(false);
  //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
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
    RedLED.startAnimation(1, 500); // Fast breathing red to indicate AP mode

    // by default DNSServer is started serving any "*" domain name. It will reply
    // AccessPoint's IP to all DNS request (this is required for Captive Portal detection)
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", WiFi.softAPIP());

    current_IP = WiFi.softAPIP();
  } else {
    current_IP = WiFi.localIP();
    strcpy(current_ssid, WiFi.SSID().c_str());
    RedLED.startAnimation(1, 4000); // Slow breathing red to indicate successful WiFi connection
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
      CameraRunning = false;
      MotorLeft.set(0); MotorRight.set(0); // Stop motors during OTA
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

  // if(WiFi.SSID() != "HACKFFM.DE") LightVars.limitLowValue = LightVars.limitHighValue; // Low light limit only for Hackerspace...

  // Start streaming web server
  startCameraServer();
  
  // Start Udp gamemaster receiver
  gmUdp.begin(gmPort);

  //analogWrite(WHITE_LED_PIN, 254); // LED low
  Serial.printf("ledcClockSource %d\n", ledcGetClockSource());
  
  
  Serial.println("===== Camera sensor info =====");
  Serial.print("Model enum: ");
  Serial.println((int)info->model);

  Serial.print("Name: ");
  Serial.println(info->name);

  sprintf(infotext, "Camera: %s", info->name);

  //WhiteLED.printInfo();
  WhiteLED.set(28);
  Serial.println("You can change the name, WiFi SSID, or password using serial commands.");
  Serial.println("Use 'name <newname>', 'ssid <newssid>' or 'password <newpassword>'.");

}

// Maximum lengths for the parsed parts (adjust if needed)
#define MAX_ID_LEN    32
#define MAX_KEY_LEN   32
#define MAX_VALUE_LEN 128

/*
 * Callback that is invoked for every parsed key=value pair.
 *
 * Parameters:
 *   id         - the id parsed at the beginning of data (either own_id or "*")
 *   key        - the name of the key
 *   value_typ  - 1 if the value is an integer (value_i is valid)
 *                2 if the value is a string (value_s is valid)
 *   value_i    - the integer value (only valid if value_typ == 1)
 *   value_s    - the string value (only valid if value_typ == 2)
 */
void gotIdKeyValue(char *id, char *key, int value_typ, int value_i, char *value_s) {
  if (value_typ == 1) {
     Serial.printf("id=%s key=%s int=%d\n", id, key, value_i);
  } else if (value_typ == 2) {
     Serial.printf("id=%s key=%s str=%s\n", id, key, value_s);
  }

  char reply[512];

  if (strcmp(key, "ping") == 0) {
    snprintf(reply, sizeof(reply), "#%s:pong=%d,rssi=%d,fps=%d,fpslim=%d,quality=%d,kbs=%d,cps=%d", roboter_name, value_i, 
    current_rssi, fps, frame_limit_ms?1000/frame_limit_ms:0, quality, bps/1024, cps);
    gmReplyToSender(reply); // answer the sender directly
  }
  if((strcmp(key, "fileRead") == 0) && (value_typ == 2)) {
    snprintf(reply, sizeof(reply), "#%s:file=\"%s\",content=\"", roboter_name, value_s);
    int l = strlen(reply);
    readFile(value_s, (char *)(reply+l), 500 - l);
    l = strlen(reply);
    reply[l] = '\"'; reply[l+1] = 0;
    Serial.println(reply);
    gmReplyToSender(reply);
  }
  if((strcmp(key, "boostTimeMax") == 0) && (value_typ == 1)) {  
    LightVars.boostTimeMax = value_i;
  }
  if((strcmp(key, "boostTime") == 0) && (value_typ == 1)) {  
    LightVars.boostTime = value_i;
  }
  if((strcmp(key, "limitLowValue") == 0) && (value_typ == 1)) {  
    LightVars.limitLowValue = value_i;
  }  
  if((strcmp(key, "limitHighValue") == 0) && (value_typ == 1)) {  
    LightVars.limitHighValue = value_i;
  } 
  if((strcmp(key, "fps") == 0) && (value_typ == 1)) {  
    cameraConfig.fps = 4; // 15 fps
    frame_limit_ms = value_i > 0 ? 1000 / value_i : 0;
  } 
  if((strcmp(key, "quality") == 0) && (value_typ == 1)) {  
    cameraConfig.quality = 2; // 42
    quality = constrain(value_i,4,63);
  }   
  if((strcmp(key, "framesize") == 0) && (value_typ == 1)) {  
    sensor_t * s = esp_camera_sensor_get();
    s->set_framesize(s, (framesize_t)constrain(value_i,0,24));
    camera_sensor_info_t *info = esp_camera_sensor_get_info(&s->id);
    if((info->model == CAMERA_OV3660)) {
      s->set_pll(s, 0, 25, 1, 0, 0, 0, 1, 10); // pushes 8MHz ext to same internally as 20mhz before
      s->set_reg(s, 0x302c, 0xc0, 0x00); // Reduce pad driving strength for better EMI/radio 
      s->set_reg(s, 0x6706, 0x0f, 0x03); // Adjust temperature sampling frequency to 8 MHz XVCLK
    } else if(s->id.PID == OV2640_PID) {
      s->set_reg(s, 0x111, 0xff, 0x80); // activate clock doubler to compensate 8 Mhz 
    }
  } 
}

/*
 * Parse a string of the form:
 *   id:key1=value,key2="value",key3=value
 *
 * The function only parses if the id at the start of data equals own_id or "*".
 * For every key=value pair found, gotIdKeyValue() is called.
 *
 * Parameters:
 *   own_id - our own id (used to decide whether the data is meant for us)
 *   data   - the 0-terminated input string to parse
 *
 * Example call:
 *   parseIdKeyValue("device1", "device1:temp=23,name=\"sensor A\",mode=2");
 */
void parseIdKeyValue(const char *own_id, const char *data) {
  if (own_id == NULL || data == NULL) {
    return;
  }

  // --- Step 1: extract the id (everything up to the first ':') ---
  const char *colon = strchr(data, ':');
  if (colon == NULL) {
    return; // no ':' found -> invalid format
  }

  size_t id_len = (size_t)(colon - data);
  if (id_len >= MAX_ID_LEN) {
    return; // id too long for our buffer
  }

  char id[MAX_ID_LEN];
  memcpy(id, data, id_len);
  id[id_len] = '\0';

  // --- Step 2: check if this data is meant for us ---
  // Accept if id matches own_id or if id is "*".
  if (strcmp(id, "*") != 0 && strcmp(id, own_id) != 0) {
    return; // not for us
  }

  // --- Step 3: parse the key=value pairs after the colon ---
  const char *p = colon + 1; // start right after the ':'

  while (*p != '\0') {
    // Skip leading separators / whitespace before a key
    while (*p == ',' || *p == ' ') {
      p++;
    }
    if (*p == '\0') {
      break;
    }

    // --- Parse key (until '=') ---
    const char *key_start = p;
    while (*p != '\0' && *p != '=' && *p != ',') {
      p++;
    }
    if (*p != '=') {
      // No '=' found -> malformed, stop parsing
      break;
    }

    size_t key_len = (size_t)(p - key_start);
    if (key_len == 0 || key_len >= MAX_KEY_LEN) {
      break; // empty or too long key -> abort
    }

    char key[MAX_KEY_LEN];
    memcpy(key, key_start, key_len);
    key[key_len] = '\0';

    p++; // skip the '='

    // --- Parse value ---
    char value_s[MAX_VALUE_LEN];
    int value_typ = 0;
    int value_i = 0;

    if (*p == '"') {
      // --- Quoted string value ---
      p++; // skip opening quote
      const char *val_start = p;
      // Read until closing quote or end of string
      while (*p != '\0' && *p != '"') {
        p++;
      }
      size_t val_len = (size_t)(p - val_start);
      if (val_len >= MAX_VALUE_LEN) {
        val_len = MAX_VALUE_LEN - 1; // truncate to fit the buffer
      }
      memcpy(value_s, val_start, val_len);
      value_s[val_len] = '\0';

      if (*p == '"') {
        p++; // skip closing quote
      }
      value_typ = 2; // string value

    } else {
      // --- Numeric (unquoted) value ---
      const char *val_start = p;
      while (*p != '\0' && *p != ',') {
        p++;
      }
      size_t val_len = (size_t)(p - val_start);
      if (val_len >= MAX_VALUE_LEN) {
        val_len = MAX_VALUE_LEN - 1; // truncate to fit the buffer
      }
      memcpy(value_s, val_start, val_len);
      value_s[val_len] = '\0';

      // Convert the numeric string to an integer
      value_i = atoi(value_s);
      value_typ = 1; // integer value
    }

    // --- Step 4: report the parsed pair ---
    gotIdKeyValue(id, key, value_typ, value_i, value_s);

    // The loop start will skip any following ',' separators.
  }
}

// Buffer for one incoming UDP packet.
// Size it large enough for your longest expected message.
#define GM_UDP_BUF_LEN 512

/*
 * Poll the gamemaster UDP socket and feed any received packet
 * into the id/key/value parser.
 */
void handleGmUdp() {
  int packetSize = gmUdp.parsePacket();
  if (packetSize <= 0) return; // nothing received
  // Store sender address/port for a possible direct reply.
  gmLastRemoteIp    = gmUdp.remoteIP();
  gmLastRemotePort  = gmUdp.remotePort();
  gmLastRemoteValid = true;

  char buf[GM_UDP_BUF_LEN];

  // Read the packet into our buffer, leaving room for the 0-terminator.
  int len = gmUdp.read(buf, GM_UDP_BUF_LEN - 1);
  if (len < 0) return;
  buf[len] = '\0'; // make it a proper C-string

  parseIdKeyValue(roboter_name, buf);
}

/*
 * Send a reply directly back to the sender of the last received packet.
 * Returns true on success, false if no valid sender is known.
 *
 * Example:
 *   gmReplyToSender("device1:ack=1");
 */
bool gmReplyToSender(const char *msg) {
  if (!gmLastRemoteValid) return false; // we have not received anything yet
  gmUdp.beginPacket(gmLastRemoteIp, gmLastRemotePort);
  gmUdp.write((const uint8_t *)msg, strlen(msg));
  return gmUdp.endPacket() == 1; // 1 = success
}


void powerDown() {
  WhiteLED.set(0); 
  RedLED.set(20);
  MotorLeft.end();
  MotorRight.end();
  Servo1.end();
  Servo2.end();
  CameraRunning = false;
  delay(1000);
  httpd_stop(camera_httpd);
  httpd_stop(stream_httpd);
  esp_camera_deinit();
  digitalWrite(XCLK_GPIO_NUM, LOW);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  esp_wifi_deinit();
  RedLED.set(0);
  Serial.println("Power down!");
  Serial.flush(); 
  delay(100);
  gpio_deep_sleep_hold_en();
  esp_deep_sleep_start();
}

int WiFiScanState = 0; // 0 = idle, 1 = scanning, 2 = scan done



/**
 * ROAMING DOES NOT WORK YET FOR WHATEVER REASON...
 * Call this cyclically from loop().
 *
 * @param forceScan  true  => start a scan immediately (if none running)
 *                   false => scan only when RSSI < SCAN_RSSI_THRESHOLD
 *                            and the SCAN_INTERVAL has elapsed.
 * @return true if a roaming switch or a hard reconnect was triggered.
 *
 * Behaviour:
 *  - Scans only the channels listed in ROAM_SCAN_CHANNELS (custom
 *    single-channel async scans chained together) to minimise airtime.
 *  - If a same-SSID AP is HYSTERESIS_DB stronger, it reassociates to it
 *    keeping the IP / DHCP lease (fast roam). Uses 802.11r FT if available.
 *  - A gateway-reachability watchdog runs in parallel. If the default
 *    gateway is unreachable for GW_FAIL_TIMEOUT_MS, a full "hard" reconnect
 *    is performed (full scan, fresh DHCP, radio re-init).
 */
bool wifiRoamTask(bool forceScan) {
    // ---------------- Configuration ----------------
    constexpr int8_t   SCAN_RSSI_THRESHOLD = -75;   // dBm
    constexpr int8_t   HYSTERESIS_DB       = 12;    // dB better required
    constexpr uint32_t SCAN_INTERVAL_MS    = 30000; // 30 s between auto-scans
    constexpr uint32_t SWITCH_COOLDOWN_MS  = 10000; // calm period after a roam
    constexpr uint32_t PER_CHANNEL_MS      = 300;   // dwell time per channel

    // Watchdog config
    constexpr uint32_t GW_PING_INTERVAL_MS = 10000;  // how often to probe GW
    constexpr uint32_t GW_FAIL_TIMEOUT_MS  = 30000;  // unreachable -> hard reset
    constexpr uint16_t GW_PROBE_PORT       = 80;    // TCP SYN target port

    // ---------------- Scan state machine ----------------
    enum ScanState { SCAN_IDLE, SCAN_RUNNING };
    static ScanState scanState   = SCAN_IDLE;
    static uint32_t  lastScanMs   = 0;
    static uint32_t  lastSwitchMs = 0;
    static size_t    chanIdx      = 0;   // index into ROAM_SCAN_CHANNELS

    // Accumulated best candidate across the per-channel scans
    static int       bestRssiAcc  = -128;
    static uint8_t   bestBssidAcc[6];
    static int32_t   bestChanAcc  = 0;
    static bool      candidateFound = false;

    // ---------------- Watchdog state ----------------
    static uint32_t  lastGwOkMs    = 0;
    static uint32_t  lastGwProbeMs = 0;

    const uint32_t now = millis();

    // =====================================================
    // 0) If not connected, let the (hard) reconnect logic
    //    handle it. Abort any running scan cleanly.
    // =====================================================
    if (WiFi.status() != WL_CONNECTED) {
        if (scanState == SCAN_RUNNING) {
            WiFi.scanDelete();
            scanState = SCAN_IDLE;
        }
        // Reset watchdog timer so we don't immediately hard-reset again.
        //lastGwOkMs = now;
        //return false;
        lastGwProbeMs = now; // Don't ping, but try reconnect
    }

    // =====================================================
    // 1) Gateway watchdog (non-blocking TCP SYN probe)
    // =====================================================
    if (lastGwOkMs == 0) lastGwOkMs = now;   // init on first connected call

    if (now - lastGwProbeMs >= GW_PING_INTERVAL_MS) {
        lastGwProbeMs = now;

        IPAddress gw = WiFi.gatewayIP();
        if (gw != IPAddress(0, 0, 0, 0)) {
            // Non-blocking connect attempt to the gateway.
            int s = lwip_socket(AF_INET, SOCK_STREAM, 0);
            if (s >= 0) {
                // set non-blocking
                int flags = lwip_fcntl(s, F_GETFL, 0);
                lwip_fcntl(s, F_SETFL, flags | O_NONBLOCK);

                struct sockaddr_in addr = {};
                addr.sin_family = AF_INET;
                addr.sin_port   = htons(GW_PROBE_PORT);
                addr.sin_addr.s_addr = (uint32_t)gw;

                lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));

                // Quick select to see if it connects (or gets refused,
                // which still proves L3 reachability).
                fd_set wset, eset;
                FD_ZERO(&wset); FD_ZERO(&eset);
                FD_SET(s, &wset); FD_SET(s, &eset);
                struct timeval tv = { 0, 200000 }; // 200 ms max
                int r = lwip_select(s + 1, NULL, &wset, &eset, &tv);

                bool reachable = false;
                if (r > 0) {
                    // Either connected or RST received => host is alive.
                    int err = 0; socklen_t len = sizeof(err);
                    lwip_getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &len);
                    // ECONNREFUSED also means the gateway answered.
                    if (err == 0 || err == ECONNREFUSED) reachable = true;
                }
                lwip_close(s);

                if (reachable) lastGwOkMs = now; else
                  Serial.printf("Gateway probe: %sreachable (r=%d, err=%d)\n",
                      reachable ? "" : "NOT ", r, errno);
            }
        } else {
            // No gateway known yet -> treat as ok to avoid false resets.
            lastGwOkMs = now;
        }
    }

    // Watchdog tripped -> hard reconnect (slow but robust)
    if (now - lastGwOkMs >= GW_FAIL_TIMEOUT_MS) {
        Serial.printf("Gateway unreachable for %lu ms -> hard reconnect",
              (unsigned long)(now - lastGwOkMs));

        if (scanState == SCAN_RUNNING) {
            WiFi.scanDelete();
            scanState = SCAN_IDLE;
        }

        // Tear everything down, re-init the radio, fresh DHCP, full scan.
        WiFi.persistent(false);
        WiFi.disconnect(true, true);     // disconnect + erase config
        WiFi.mode(WIFI_OFF);
        delay(50);
        esp_wifi_stop();                 // stop driver -> radio recalibration
        delay(50);
        esp_wifi_start();
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);

        // Reconnect using the stored SSID/password from NVS or your globals.
        WiFi.begin(wifi_ssid, wifi_password);   // full scan + fresh DHCP

        lastGwOkMs    = now;   // reset watchdog window
        lastSwitchMs  = now;
        lastScanMs    = now;
        return true;
    }

    // =====================================================
    // 2) Scan trigger logic
    // =====================================================
    if (scanState == SCAN_IDLE &&
        (now - lastSwitchMs > SWITCH_COOLDOWN_MS)) {

        bool startScan = false;
        if (forceScan) {
            startScan = true;
        } else if ((now - lastScanMs) >= SCAN_INTERVAL_MS) {
            if (WiFi.RSSI() < SCAN_RSSI_THRESHOLD) {
                startScan = true;
            } else {
                lastScanMs = now;   // RSSI fine, just reset the tick
            }
        }

        if (startScan) {
            // Reset accumulators and start scanning the first channel.
            chanIdx        = 0;
            bestRssiAcc    = WiFi.RSSI() + HYSTERESIS_DB; // must be beaten
            candidateFound = false;
            lastScanMs     = now;

            if (ROAM_SCAN_CHANNEL_COUNT == 0) {
                // No list given -> scan all channels in one async scan.
                WiFi.scanNetworks(true, false, false, PER_CHANNEL_MS, 0);
                Serial.printf("Starting full async scan (all channels, %d ms dwell)\n", PER_CHANNEL_MS);
            } else {
                // Start with the first channel from our list.
                WiFi.scanNetworks(true, false, false, PER_CHANNEL_MS,
                                  ROAM_SCAN_CHANNELS[chanIdx]);
                Serial.printf("Starting async scan on channel %d (dwell %d ms)\n", ROAM_SCAN_CHANNELS[chanIdx], PER_CHANNEL_MS);
            }
            scanState = SCAN_RUNNING;
        }
    }

    // =====================================================
    // 3) Scan result handling (per-channel chaining)
    // =====================================================
    if (scanState == SCAN_RUNNING) {
        int n = WiFi.scanComplete();
        
        if (n == WIFI_SCAN_RUNNING) return false;   // -1: still scanning

        if (n == WIFI_SCAN_FAILED) {                // -2: start failed/busy
            // The driver rejected the scan (often busy with the video
            // stream). Clean up and RETRY the SAME channel next call,
            // do NOT advance chanIdx.
            static uint8_t failRetries = 0;
            WiFi.scanDelete();

            if (++failRetries >= 5) {
                // Give up this scan round to avoid an infinite stall.
                Serial.println("Scan kept failing -> aborting scan round");
                failRetries = 0;
                scanState   = SCAN_IDLE;
                lastScanMs  = now;          // back off until next interval
                return false;
            }

            // Re-arm the same channel.
            int16_t ret = WiFi.scanNetworks(
                true, false, false, PER_CHANNEL_MS,
                (ROAM_SCAN_CHANNEL_COUNT > 0) ? ROAM_SCAN_CHANNELS[chanIdx] : 0);
            Serial.printf("Scan retry ch=%d ret=%d (try %u)\n",
                (ROAM_SCAN_CHANNEL_COUNT > 0) ? ROAM_SCAN_CHANNELS[chanIdx] : 0,
                ret, failRetries);
            return false;
        }

        // n >= 0: results available -> evaluate this channel.
        if (n >= 0) {
            const String   curSsid  = WiFi.SSID();
            const uint8_t* curBssid = WiFi.BSSID();
            Serial.printf("Scan result: n=%d, ch=%d\n", n, ROAM_SCAN_CHANNELS[chanIdx]);

            for (int i = 0; i < n; ++i) {
                if (WiFi.SSID(i) != curSsid)                 continue;
                if (memcmp(WiFi.BSSID(i), curBssid, 6) == 0) continue;
                int8_t r = WiFi.RSSI(i);
                if (r > bestRssiAcc) {
                    bestRssiAcc = r;
                    memcpy(bestBssidAcc, WiFi.BSSID(i), 6);
                    bestChanAcc = WiFi.channel(i);
                    candidateFound = true;
                }
            }
        }
        WiFi.scanDelete();

        // Advance to next channel and CHECK the start return value.
        bool moreChannels = false;
        if (ROAM_SCAN_CHANNEL_COUNT > 0) {
            chanIdx++;
            if (chanIdx < ROAM_SCAN_CHANNEL_COUNT) {
                int16_t ret = WiFi.scanNetworks(
                    true, false, false, PER_CHANNEL_MS,
                    ROAM_SCAN_CHANNELS[chanIdx]);
                if (ret == WIFI_SCAN_FAILED) {
                    // Stay in SCAN_RUNNING; the -2 branch above will
                    // retry this channel on the next call.
                    Serial.printf("Next-channel scan start failed ch=%d\n",
                                  ROAM_SCAN_CHANNELS[chanIdx]);
                }
                moreChannels = true;   // keep SCAN_RUNNING either way
            }
        }

        if (moreChannels) return false;

        // ---- All requested channels scanned: decide on roaming ----
        scanState = SCAN_IDLE;

        Serial.printf("Scan done. %sandidate found. CH: %d\n", candidateFound ? " C":"No c", bestChanAcc);

        if (candidateFound) {
            wifi_config_t cfg = {};
            esp_wifi_get_config(WIFI_IF_STA, &cfg);
            memcpy(cfg.sta.bssid, bestBssidAcc, 6);
            cfg.sta.bssid_set = true;
            cfg.sta.channel   = (uint8_t)bestChanAcc;

        #if ROAM_FT_AVAILABLE
            cfg.sta.rm_enabled  = true;
            cfg.sta.btm_enabled = true;
            cfg.sta.ft_enabled  = true;
        #endif

            // Disconnect first - ESP32 WiFi stack ignores bssid_set on already-connected STA
            esp_wifi_disconnect();

            // Small delay to let the stack process the disconnect cleanly
            vTaskDelay(pdMS_TO_TICKS(100));

            esp_wifi_set_config(WIFI_IF_STA, &cfg);

            esp_err_t err = esp_wifi_connect();
            if (err == ESP_OK) {
                lastSwitchMs = now;

                // Wait for actual connection and verify target BSSID was reached
                uint32_t waitStart = millis();
                bool connected = false;
                while (millis() - waitStart < 5000) {
                    if (WiFi.status() == WL_CONNECTED) {
                        connected = true;
                        break;
                    }
                    vTaskDelay(pdMS_TO_TICKS(100));
                }

                if (connected) {
                    // Use esp_wifi_sta_get_ap_info() - more reliable than WiFi.BSSIDstr()
                    wifi_ap_record_t apInfo;
                    if (esp_wifi_sta_get_ap_info(&apInfo) == ESP_OK) {
                        bool bssidMatch = (memcmp(apInfo.bssid, bestBssidAcc, 6) == 0);
                        Serial.printf("Roam -> %02X:%02X:%02X:%02X:%02X:%02X "
                                      "ch %ld RSSI %d (FT=%d) BSSIDmatch=%d\n",
                                      apInfo.bssid[0], apInfo.bssid[1], apInfo.bssid[2],
                                      apInfo.bssid[3], apInfo.bssid[4], apInfo.bssid[5],
                                      (long)bestChanAcc, apInfo.rssi,
                                      ROAM_FT_AVAILABLE, bssidMatch);
                    }
                } else {
                    Serial.println("Roam: connect timeout, reverting bssid_set=false");
                    // Fallback: let ESP32 choose AP freely again
                    cfg.sta.bssid_set = false;
                    esp_wifi_set_config(WIFI_IF_STA, &cfg);
                    esp_wifi_connect();
                }
                return connected;
            }
        }

    }

    return false;
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
      } else if(strncmp(inputBuffer, "n ", 2) == 0) {
        strlcpy(roboter_name, inputBuffer + 2, sizeof(roboter_name));
        writeFile("/roboter_name.txt", roboter_name);
        Serial.printf("Updated name to: %s\n", roboter_name);
        Serial.println("Rebooting...");
        delay(100);
        ESP.restart();
      } else if(strncmp(inputBuffer, "ssid ", 5) == 0) {
        strlcpy(wifi_ssid, inputBuffer + 5, sizeof(wifi_ssid));
        writeFile("/wifi_ssid.txt", wifi_ssid);
        Serial.printf("Updated WiFi SSID to: %s\n", wifi_ssid);
      } else if(strncmp(inputBuffer, "password ", 9) == 0) {
        strlcpy(wifi_password, inputBuffer + 9, sizeof(wifi_password));
        writeFile("/wifi_password.txt", wifi_password);
        Serial.printf("Updated WiFi password to: %s\n", wifi_password);
      } else if(strncmp(inputBuffer, "scan", 4) == 0) {
        RoamForceTriggered = true; // Trigger WiFi scan/roam in main loop
      } else if(strncmp(inputBuffer, "off", 3) == 0) {
        powerDown();        
      } else if(strncmp(inputBuffer, "t1", 2) == 0) {
        camera_config_t config;     
        sensor_t * s = esp_camera_sensor_get();   
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

void adjust_to_rssi() {
  static uint32_t last_adjust_time = 0;
  uint32_t now = millis();
  
  static int avg_rssi_sum = 0;
  static int avg_rssi_count = 0;

  if (now - last_adjust_time >= 100) {
    // for AP mode take RSSI from 1st connected station
    int rssi = WiFi.RSSI();
    if (APMode) {
      wifi_sta_list_t stationList;
      esp_wifi_ap_get_sta_list(&stationList);
      if (stationList.num > 0) {
        rssi = stationList.sta[0].rssi;
      }
    } else {
      
    }
    strcpy(current_bssid, "N/A");
    wifi_mode_t mode = WIFI_MODE_NULL;
    if (esp_wifi_get_mode(&mode) == ESP_OK) {
      uint8_t bssid[6] = {};
      uint8_t channel  = 0;
      if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA) {
          wifi_ap_record_t ap_info = {};
          if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            memcpy(bssid, ap_info.bssid, 6);
            channel = ap_info.primary;
          }
      } else if (mode == WIFI_MODE_AP) {
          wifi_config_t cfg = {};
          if((esp_wifi_get_config(WIFI_IF_AP, &cfg) == ESP_OK) &&
             (esp_wifi_get_mac(WIFI_IF_AP, bssid) == ESP_OK)) {
            channel = cfg.ap.channel;
          }
      } 
      sprintf(current_bssid, "%02X:%02X:%02X:%02X:%02X:%02X (ch %d)",
              bssid[0], bssid[1], bssid[2],
              bssid[3], bssid[4], bssid[5],
              channel);
      current_channel = channel;
    }
    avg_rssi_sum += rssi;
    avg_rssi_count++;
    if(avg_rssi_count >= 10) { // Update every 10 samples
      int avg_rssi = avg_rssi_sum / avg_rssi_count;
      current_rssi = avg_rssi;

      // Adjust camera settings based on average RSSI if set to auto
      // currently only for 640x480
      if(cameraConfig.fps == 0) {
        if(avg_rssi > -55) {
          frame_limit_ms = 1000 / 25; // Up to 25 fps
          if(cameraConfig.quality == 0) quality = 18; // 6 is too crazy for 25 fps
        } else if(avg_rssi > -60) {
          frame_limit_ms = 1000 / 20; // Up to 20 fps
          if(cameraConfig.quality == 0) quality = 30;
        } else if(avg_rssi > -70) {
          frame_limit_ms = 1000 / 15; // Up to 15 fps
          if(cameraConfig.quality == 0) quality = 45;
        } else {
          frame_limit_ms = 1000 / 7; // Up to 7 fps
          if(cameraConfig.quality == 0) quality = 60;
        }
      }
       
      avg_rssi_sum = 0;
      avg_rssi_count = 0;
    }
    last_adjust_time = now;
  }
}

void adjust_light() {
  static int prevValue = -2;
  static uint32_t lastBoostUpdateTime = 0;
  uint32_t now = millis();
  uint32_t timeDiff = now - lastBoostUpdateTime;

  // Update boost time, decrease normally when LED is on, increase by 20ms/s when LED is off or low to allow boost again after cooldown
  if(timeDiff >= 100) {
    if(LightVars.Value > LightVars.limitLowValue) {
      if(LightVars.boostTime > 0) {
        LightVars.boostTime -= timeDiff; // Decrease boost time when LED is on high
      }
    } else {
      if(LightVars.boostTime < LightVars.boostTimeMax) {
        LightVars.boostTime += (timeDiff * 50) / 1000; // Increase boost time by 50ms/s when LED is off or low
      }
    }
    lastBoostUpdateTime = now;
    // Limit light level based on boost time
    if(LightVars.boostTime > 1000) {
      LightVars.Value = min(LightVars.Value, LightVars.limitHighValue); 
    } else {
      LightVars.Value = min(LightVars.Value, LightVars.limitLowValue); 
    }
  }
  
  // Set new value if requested
  if(LightVars.requestedValue != -1) {
    if(LightVars.boostTime > 1000) {
      LightVars.Value = min(LightVars.requestedValue, LightVars.limitHighValue); 
    } else {
      LightVars.Value = min(LightVars.requestedValue, LightVars.limitLowValue); 
    } 
    LightVars.requestedValue = -1;
  }

  // Adjust LED brightness on change of value
  if(LightVars.Value != prevValue) {
     prevValue = LightVars.Value;
     WhiteLED.set(LightVars.Value);
  }
}

void loop() {
  if(OTA_Status < 2) {
    calc_fps();
    adjust_to_rssi();
  }

  #ifdef ENABLE_OTA
  ArduinoOTA.handle(); // allow OTA updates
  #endif

  if(APMode) dnsServer.processNextRequest(); // Handle DNS requests in AP mode for captive portal

  RedLED.doAnimation();
  adjust_light();

  static uint32_t limitServoUpdate = millis();
  if(millis() - limitServoUpdate > 40) {
    limitServoUpdate = millis();
    Servo1.doAnimation();
  }

  if(lastMotorCommandTime > 0 && (millis() - lastMotorCommandTime > 3000)) {
    MotorLeft.set(0);
    MotorRight.set(0);
    lastMotorCommandTime = 0;
    Serial.println("Motor command timeout, stopping motors");
  }

  // Auto power down
  if(lastMotorCommandTime == 0) {
    if((powerDownTimeout > 0) && (millis() - powerDownTimer > powerDownTimeout)) {
      powerDown();
    }
  } else {
    powerDownTimer = millis(); // Reset power down timer when motor command is active
  }
  if(powerDownTimeout == 0xfffffffful) { powerDown();}

  processSerial(); // Check for serial commands 
  handleGmUdp();

  if(!APMode) {
    wifiRoamTask(RoamForceTriggered); // Check if we should roam to a better AP
    RoamForceTriggered = false; // Reset roam trigger after handling
  }

  delay(10);
}

