/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-cam-projects-ebook/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

const char* ROBOTER_NAME = "cambot-proto1"; // only use a-z, 0-9 and - in the name 

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
#    include <MyCredsHackffm.h>  // Define WIFI_SSID and WIFI_PASSWORD here - see file in Attic for example
#  endif
#endif

// Replace with your network credentials
const char* ssid = WIFI_SSID;  // "REPLACE_WITH_YOUR_SSID";
const char* password = WIFI_PASSWORD; // "REPLACE_WITH_YOUR_PASSWORD";

#include "esp_camera.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems
#include "esp_http_server.h"

float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) { return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow; }

#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM_B
//#define CAMERA_MODEL_WROVER_KIT

#include "cam_pindefs.h"
#include "cam_streamhandler.h"
#include "indexhtml8.h"

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

static esp_err_t index_handler(httpd_req_t *req){
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

static esp_err_t info_handler(httpd_req_t *req){
  char info[512];
  snprintf(info, sizeof(info), "Free heap: %u bytes, WiFi RSSI: %d dBm, FPS: %d, kBytes/s: %d ", ESP.getFreeHeap(), WiFi.RSSI(), fps, bps/1024);
  snprintf(info + strlen(info), sizeof(info) - strlen(info), "| Name=\"%s\", A=\"FPS-Limit (%d ms)\", B=\"Quality\", C=\"LED\", D=\"Servo1\", E=\"Servo2\" ", ROBOTER_NAME, frame_limit_ms);
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

  sensor_t * s = esp_camera_sensor_get();
  float x = key_values[0]/127.0; // Convert to -1.0 to 1.0
  float y = key_values[1]/127.0; // Convert to -1.0 to 1.0
  float a = key_values[2]/127.0; // Convert to -1.0 to 1.0
  float b = key_values[3]/127.0; // Convert to -1.0 to 1.0
  float c = key_values[4]/127.0; // Convert to -1.0 to 1.0
  float motor_left = y + x;
  float motor_right = y - x;

  frame_limit_ms = mapFloat(a, -1.0, 1.0, 25.0, 250.0); 
  analogWrite(4, constrain(c * 255, 0, 255)); // LED brightness control
  int quality = constrain(((1.0-b)/2.0)*63,0, 63);  // 0...63 lower=higher quality
  static int prev_quality = 10;
  if(quality != prev_quality) { s->set_quality(s, quality); prev_quality = quality; }

  #ifdef MOTOR_LOW_IDLE
    int mot_l1 = constrain(motor_left * 255, 0, 255);
    int mot_l2 = constrain(-motor_left * 255, 0, 255);
    int mot_r1 = constrain(motor_right * 255, 0, 255);
    int mot_r2 = constrain(-motor_right * 255, 0, 255);
  #else
    int mot_l1 = constrain(-motor_left * 255, 0, 255);
    int mot_l2 = constrain(motor_left * 255, 0, 255);
    int mot_r1 = constrain(-motor_right * 255, 0, 255);
    int mot_r2 = constrain(motor_right * 255, 0, 255);

    if(mot_l1 == 0) mot_l1 = 255; else mot_l1 = 255 - mot_l1;
    if(mot_l2 == 0) mot_l2 = 255; else mot_l2 = 255 - mot_l2;
    if(mot_r1 == 0) mot_r1 = 255; else mot_r1 = 255 - mot_r1;
    if(mot_r2 == 0) mot_r2 = 255; else mot_r2 = 255 - mot_r2;
  #endif

  analogWrite(MOTOR_1_PIN_1, mot_l1);
  analogWrite(MOTOR_1_PIN_2, mot_l2);
  analogWrite(MOTOR_2_PIN_1, mot_r1);
  analogWrite(MOTOR_2_PIN_2, mot_r2);
  
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
  
  pinMode(MOTOR_1_PIN_1, OUTPUT);
  pinMode(MOTOR_1_PIN_2, OUTPUT);
  pinMode(MOTOR_2_PIN_1, OUTPUT);
  pinMode(MOTOR_2_PIN_2, OUTPUT);
  
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  uint32_t psramSize = psramFound() ? ESP.getPsramSize() : 0;
  Serial.printf("Reset: PSRAM: %uMB, Flash: %uMB\n", psramSize / (1024 * 1024), ESP.getFlashChipSize() / (1024 * 1024));
  
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
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  // Wi-Fi connection
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(ROBOTER_NAME); 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi connected. Camera Stream Ready! Go to: http://%s or http://%s.local\n", WiFi.localIP().toString().c_str(), ROBOTER_NAME);
  MDNS.begin(ROBOTER_NAME);
  MDNS.addService("_http", "_tcp", 80);
  
  sensor_t * s = esp_camera_sensor_get();
  s->set_hmirror(s, 1);
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable

  // Start streaming web server
  startCameraServer();
}

void loop() {
  calc_fps();
}
