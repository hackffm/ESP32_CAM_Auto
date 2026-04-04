// Part that sends the camera picture as MJPEG stream.
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "\r\n--" PART_BOUNDARY "\r\n" "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

int quality = 20;
int fps = 0; int fps_count = 0; 
int bps = 0; int bps_count = 0;
int cps = 0; int cps_count = 0;
uint32_t last_fps_time = 0; // checked in main_loop
uint32_t last_frame_time = 0; // checked in main_loop
uint32_t frame_limit_ms = 50; // default to 20 fps, can be set from web interface
int  camera_temp = 0;

void calc_fps() {
  uint32_t now = millis();
  if (now - last_fps_time >= 1000) {
    fps = fps_count; fps_count = 0;
    bps = bps_count; bps_count = 0;
    cps = cps_count; cps_count = 0;
    last_fps_time = now;
    //Serial.printf("FPS: %d, BPS: %d\n", fps, bps);
    sensor_t * s = esp_camera_sensor_get();
    if(s->id.PID == OV3660_PID) {
      camera_temp = (int)s->get_reg(s, 0x6719, 0xff);
    }
  }
}

static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;
  struct timeval _timestamp;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[180];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    Serial.println("Failed to set stream content type");
    return res;
  }
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, OPTIONS");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "*");
  httpd_resp_set_hdr(req, "X-Framerate", "60");

  while(OTA_Status < 2){
    uint32_t now = millis();
    if((uint32_t)(now - last_frame_time) < (uint32_t)frame_limit_ms) {
      delay(frame_limit_ms - (uint32_t)(now - last_frame_time));
    }
    last_frame_time = millis();
    
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if(fb->width > 100){
        _timestamp.tv_sec = fb->timestamp.tv_sec;
        _timestamp.tv_usec = fb->timestamp.tv_usec;
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, quality, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          } 
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if(res == ESP_OK){
      //size_t hlen = snprintf((char *)part_buf, 180, _STREAM_PART, _STREAM_BOUNDARY, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
      size_t hlen = snprintf((char *)part_buf, 180, _STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
      if(res != ESP_OK) Serial.println("Failed to send stream header");
    } 
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
      if(res != ESP_OK) Serial.println("Failed to send stream data");
    } 

    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if(res != ESP_OK){
      break;
    }
    //Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
    fps_count++; bps_count += _jpg_buf_len;
  }
  return res;
}
