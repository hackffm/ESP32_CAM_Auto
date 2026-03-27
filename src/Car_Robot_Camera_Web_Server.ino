/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-cam-projects-ebook/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

// GPIO12 Gelb
// GPIO13 Grün
// GPIO15 Braun
// GPIO14 Violett
// GPIO2  Grau
// GPIO4  LED
#define MOTOR_1_PIN_1    14
#define MOTOR_1_PIN_2    15
#define MOTOR_2_PIN_1    13
#define MOTOR_2_PIN_2    12
#define SERVO_1_PIN       2
#define WHITE_LED_PIN     4
#define RED_LED_PIN      33
//#define MOTOR_LOW_IDLE 1 // Check what gives better power to motor - with or without this flag

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
char roboter_name[34] = "cam-dev2"; // only use a-z, 0-9 and - in the name 
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

PwmThing MotorLeft, MotorRight, WhiteLED, RedLED, Servo1;

WiFiMulti wifiMulti;

float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) { return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow; }

#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM_B
//#define CAMERA_MODEL_WROVER_KIT

#include "cam_pindefs.h"
#include "cam_streamhandler.h"
// #include "indexhtml8.h"  // not needed anymore, now in /data/index.html and served from LittleFS

uint32_t WhiteLedMaxOnTimeMs = 30000; // Prevent LED overheating
int      WhiteLedTimeoutThresholdValue = 40; // Count down MaxOnTimeMs above this threshold, limit LED to this threshold afterwards

fs::FS &filesystem = LittleFS;

// Function to write a string to a file
bool writeFile(const char *path, const char *data) {
  File file = filesystem.open(path, "w", true); // "w" overwrite file
  if (!file) {
    Serial.println("Can't open file to write");
    return false;
  }
  file.print(data);
  file.close();
  Serial.println("File written");
  return true;
}

// Function to read a file and return the content as a string
size_t readFile(const char *path, char *buffer, size_t bufferSize) {
  if(filesystem.exists(path) == false) {
    // LL_Log.printf("File %s does not exist\r\n", path);
    return 0;
  }
  File file = filesystem.open(path, "r");  
  if (!file) {
    Serial.println("Can't open file for reading");
    return 0;
  }
  size_t bytesRead = 0;
  if(file.size() > 0) {
    bytesRead = file.readBytes(buffer, bufferSize - 1); // Leave space for null terminator
    buffer[bytesRead] = '\0'; // Null-terminate the string
  }
  file.close();
  return bytesRead;
}

void listDir(const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  fs::File root = filesystem.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }

  size_t t = LittleFS.totalBytes();
  size_t u = LittleFS.usedBytes();
  
  Serial.printf("Total space: %d, used space: %d, free space: %d\r\n", t, u, t - u);
}

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

//static esp_err_t index_handler(httpd_req_t *req){
//  httpd_resp_set_type(req, "text/html");
//  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
//}

static esp_err_t index_handler(httpd_req_t *req)
{
  Serial.println("Serving index.html");
  File file = filesystem.open("/index.html", "r");
  if (!file) {
      httpd_resp_send_404(req);
      Serial.println("Failed to open index.html");
      return ESP_FAIL;
  }
  httpd_resp_set_type(req, "text/html");

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

  snprintf(info, sizeof(info), "%s, Cam-Temp: %d°C, Free heap: %u bytes, \r\nFree PSRAM: %u bytes, WiFi RSSI: %d dBm, FPS: %d, kBytes/s: %d ", infotext, camera_temp, ESP.getFreeHeap(), ESP.getFreePsram(), WiFi.RSSI(), fps, bps/1024);
  snprintf(info + strlen(info), sizeof(info) - strlen(info), "| Name=\"%s\", A=\"FPS-Limit (%d fps)\", B=\"Quality (%d)\", C=\"LED (Boost remaining: %ds)\", D=\"Servo\", E=\"E\" ", roboter_name, 1000/frame_limit_ms, quality, max(WhiteLedMaxOnTimeMs/1000,0UL));
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

  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if(!buf){
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      //Serial.printf("Received query: %s\n", buf);
      // Loop through the expected query keys and extract their values
      for (int i = 0; i < num_keys; i++) {
        if (httpd_query_key_value(buf, query_keys[i], variable, sizeof(variable)) == ESP_OK) {
          key_values[i] = atoi(variable);
          res = 0;
          //Serial.printf("Key: %s, Value: %d\n", query_keys[i], key_values[i]);
        } 
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

  float a = key_values[2]/127.0; // Convert to -1.0 to 1.0
  float b = key_values[3]/127.0; // Convert to -1.0 to 1.0
  float c = key_values[4]/127.0; // Convert to -1.0 to 1.0
  float d = key_values[5]/127.0; // Convert to -1.0 to 1.0
  float e = key_values[6]/127.0; // Convert to -1.0 to 1.0
 
 
  //servo1_target = mapFloat(d, -1.0, 1.0, 0, 180);
  //analogWrite(SERVO_1_PIN, mapFloat(d, -1.0, 1.0, 192, 2386) );

  frame_limit_ms = mapFloat(a, -1.0, 1.0, 250.0, 20.0); 
  //analogWrite(WHITE_LED_PIN, constrain(c * 255, 0, 255)); // LED brightness control
  if(c > 0) {
    WhiteLED.set(constrain(c * 255, 0, 255));
    RedLED.set(0);
  } else {
    WhiteLED.set(0);
    RedLED.set(constrain(c * -255, 0, 255));
  }
  //analogWrite(RED_LED_PIN, constrain((1.0+c) * 255, 0, 255)); // LED brightness control

  quality = constrain(((1.0-b)/2.0)*63,4, 63);  // 0...63 lower=higher quality
  static int prev_quality = 10;
  if(quality != prev_quality) { 
    sensor_t * s = esp_camera_sensor_get();
    s->set_quality(s, quality); 
    prev_quality = quality; 
  }
  



  if(res){
    return httpd_resp_send_500(req);
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
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
  
  // Initialize filesystem
  if(!LittleFS.begin()) {
    Serial.println("filesystem Mount Failed");
    return;
  }
  listDir("/", 0); // List root directory for debugging

  // try to load user data from files
  readFile("/roboter_name.txt", roboter_name, sizeof(roboter_name));
  readFile("/wifi_ssid.txt", wifi_ssid, sizeof(wifi_ssid));
  readFile("/wifi_password.txt", wifi_password, sizeof(wifi_password));
  Serial.printf("Loaded config: Name=%s, SSID=%s, Password=%s\n", roboter_name, wifi_ssid, wifi_password);

  MotorLeft.begin(MOTOR_2_PIN_1, MOTOR_2_PIN_2, PwmThing::halfBridgeIdleHigh, true);
  MotorRight.begin(MOTOR_1_PIN_1, MOTOR_1_PIN_2, PwmThing::halfBridgeIdleHigh, true);
  Servo1.begin(SERVO_1_PIN, -1, PwmThing::servoMotor);

 // MotorLeft.begin(12, -1, PwmThing::servoMotor0Stop, false);
 // MotorRight.begin(13, -1, PwmThing::servoMotor0Stop, true);
 // Servo1.begin(15, -1, PwmThing::servoMotor, false);

  WhiteLED.begin(WHITE_LED_PIN, -1, PwmThing::pwmOutGamma, false);
  RedLED.begin(RED_LED_PIN, -1, PwmThing::pwmOutGamma, true);
  
  WhiteLED.set(1); // LED off

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
  //config.fb_location = CAMERA_FB_IN_DRAM;
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 30;
    config.fb_count = 3;
  } else {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 30;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  camera_sensor_info_t *info = esp_camera_sensor_get_info(&s->id);
  if((info->model == CAMERA_OV3660)) {
    s->set_hmirror(s, 1);
    s->set_vflip(s, 0);          // 0 = disable , 1 = enable
    //s->set_xclk(s, LEDC_TIMER_0, 8000000UL);
    s->set_pll(s, 0, 25, 1, 0, 0, 0, 1, 10); // pushes 8MHz ext to same internally as 20mhz before
    s->set_reg(s, 0x302c, 0xc0, 0x00); // Reduce pad driving strength for better EMI/radio
    s->set_reg(s, 0x6706, 0x0f, 0x03); // Adjust temperature sampling frequency to 8 MHz XVCLK
  } else if(s->id.PID == OV2640_PID) {
    s->set_gainceiling(s, (gainceiling_t)2); // default is 2x, set to 1x to reduce noise
    s->set_reg(s, 0x111, 0xff, 0x80); // activae clock doubler to compensate 8 Mhz 
  }

  // Wi-Fi connection
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(roboter_name); 
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  wifiMulti.addAP(wifi_ssid, wifi_password);
  WhiteLED.set(32); // LED on
  Serial.printf("Connecting to WiFi SSID: %s\n", wifi_ssid);

  if(wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi, opening AP mode.");
    WiFi.AP.begin();
    WiFi.AP.create(roboter_name);
    WiFi.AP.enableDhcpCaptivePortal();

    // by default DNSServer is started serving any "*" domain name. It will reply
    // AccessPoint's IP to all DNS request (this is required for Captive Portal detection)
    if (dnsServer.start()) {
      Serial.println("Started DNS server in captive portal-mode");
    } else {
      Serial.println("Err: Can't start DNS server!");
    }
  }
  //esp_wifi_set_max_tx_power(40);

  WiFi.setSleep(false);
  Serial.printf("\nWiFi connected. Camera Stream Ready! Go to: http://%s or http://%s.local\n", WiFi.localIP().toString().c_str(), roboter_name);
  MDNS.begin(roboter_name);
  MDNS.addService("_http", "_tcp", 80);
  #ifdef ENABLE_OTA
    ArduinoOTA.onStart([]() {
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating ");
      WhiteLED.set(1); 
      RedLED.set(255); // Red LED on to indicate OTA in progress
      OTA_Status = 2; // Set status to in progress
    });
    ArduinoOTA.setHostname(roboter_name);
    OTA_Status = 1;
    ArduinoOTA.begin();  
  #endif


  // Start streaming web server
  startCameraServer();
  
  //analogWrite(WHITE_LED_PIN, 254); // LED low
  Serial.printf("ledcClockSource %d\n", ledcGetClockSource());
  
  
  Serial.println("===== Camera sensor info =====");
  Serial.print("Model enum: ");
  Serial.println((int)info->model);

  Serial.print("Name: ");
  Serial.println(info->name);

  sprintf(infotext, "BSSID: %s, Camera: %s", WiFi.BSSIDstr().c_str(), info->name);

  WhiteLED.printInfo();
  WhiteLED.set(28);

}

void loop() {
  calc_fps();

  #ifdef ENABLE_OTA
  ArduinoOTA.handle(); // allow OTA updates
  if(OTA_Status == 2) { // All power to OTA updates
    esp_camera_deinit();
    while(1) { delay(1); ArduinoOTA.handle(); }
  }
  #endif

  if(WhiteLED.get() > WhiteLedTimeoutThresholdValue) {
    if(WhiteLedMaxOnTimeMs > 0) {
      WhiteLedMaxOnTimeMs -= 10; // Count down timer
    } else {
      WhiteLED.set(WhiteLedTimeoutThresholdValue); // Limit LED to threshold value to prevent overheating
    }
  } 

  delay(10);
}
