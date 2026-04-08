#include <lvgl.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include "ui.h"
#include "screens.h"
#include "actions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================
// HARDWARE PINS & SETTINGS
// =========================================================
#define SD_MOSI 11
#define SD_MISO 13
#define SD_SCK 12
#define SD_CS 10

#define TFT_BL 2
#define GFX_BL DF_GFX_BL 

#define RX_PIN 18
#define TX_PIN 17

const int pwmFreq = 5000;
const int pwmChannel = 0;
const int pwmResolution = 8;

HardwareSerial testingSerial(1);

char display_state[10][32] = {0};

// =========================================================
// DISPLAY & TOUCH SETUP
// =========================================================
SPIClass& spi = SPI;

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
  GFX_NOT_DEFINED, GFX_NOT_DEFINED, GFX_NOT_DEFINED,
  40, 41, 39, 42, 45, 48, 47, 21, 14,
  5, 6, 7, 15, 16, 4, 8, 3, 46, 9, 1
);

Arduino_RPi_DPI_RGBPanel *lcd = new Arduino_RPi_DPI_RGBPanel(
  bus, 480, 0, 8, 4, 43, 272, 0, 8, 4, 12, 1, 7000000, true);

#include "touch.h"

static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[480 * 272 / 8];
static lv_disp_drv_t disp_drv;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  lcd->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (touch_has_signal()) {
    if (touch_touched()) {
      data->state = LV_INDEV_STATE_PR;
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    } else if (touch_released()) {
      data->state = LV_INDEV_STATE_REL;
    }
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
} 

float parse_float_robust(const char* str) {
    if (str == NULL || strlen(str) == 0) return 0.0f;
    char temp[20] = {0};
    int i = 0;
    bool found_digit = false;
    for (int j = 0; str[j] != '\0'; j++) {
        char c = str[j];
        if (c >= '0' && c <= '9') {
            temp[i++] = c;
            found_digit = true;
        } else if (c == '.' || c == '-') {
            if (!found_digit && c == '-') temp[i++] = c;
            else if (c == '.') temp[i++] = c;
        } else if (found_digit) {
            break;
        }
        if (i >= 19) break; 
    }
    if (!found_digit) return 0.0f;
    return atof(temp);
}

int get_7s_soc(float voltage) {
    if (voltage >= 29.4f) return 100;
    if (voltage <= 21.0f) return 0;
    return (int)(((voltage - 21.0f) / (29.4f - 21.0f)) * 100.0f);
}

// =========================================================
// MAIN ARDUINO SETUP
// =========================================================
void setup() {
  Serial.begin( 115200 ); 
  
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmChannel);
  ledcWrite(pwmChannel, 255); 

  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(0, OUTPUT);

  lv_init();
  lcd->begin();
  lcd->fillScreen(BLACK);

  touch_init();

  screenWidth = lcd->width();
  screenHeight = lcd->height();
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 8);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.sw_rotate = 1;                    
  disp_drv.rotated   = LV_DISP_ROT_90;       
  disp_drv.flush_cb  = my_disp_flush;
  disp_drv.draw_buf  = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  delay(300);

  lv_disp_t * disp = lv_disp_get_default();
  lv_theme_t * theme = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, theme);

  ui_init();
  testingSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
}

// =========================================================
// MAIN ARDUINO LOOP
// =========================================================
void loop() {
  lv_timer_handler();
  lv_tick_inc(5);
  delay(5);

  static String buffer = "";
  
  static float ema_values[6] = {0};
  static bool first_reading[6] = {true, true, true, true, true, true};
  const float EMA_BETA = 2.0f / (10.0f + 1.0f); 

  while (testingSerial.available() > 0) {

    char c = testingSerial.read();

    if (c == '\n') {
      buffer.trim();
      if (buffer.length() > 0) {
        char prefix = buffer[0]; 
        
        if (prefix >= 'A' && prefix <= 'F') {
          int index = prefix - 'A';
          
          String valueStr = buffer.substring(1);
          valueStr.trim();
          float new_value = parse_float_robust(valueStr.c_str());
          
          if (first_reading[index]) {
              ema_values[index] = new_value; 
              first_reading[index] = false;
          } else {
              ema_values[index] = (1.0f - EMA_BETA) * ema_values[index] + (EMA_BETA * new_value);
          }

          char tempStr[32];
          if (index == 0 || index == 1) { 
              snprintf(tempStr, sizeof(tempStr), "%.2f A", ema_values[index]);
          } else if (index == 2) { 
              snprintf(tempStr, sizeof(tempStr), "%.1f C", ema_values[index]);
          } else { 
              snprintf(tempStr, sizeof(tempStr), "%.2f V", ema_values[index]);
          }

          strncpy(display_state[index], tempStr, 31);
          display_state[index][31] = '\0';
        }
      }
      buffer = "";
    }
    else if (c != '\r' && c != 0) {
      buffer += c;
      if (buffer.length() > 40) buffer = buffer.substring(buffer.length() - 40); 
    }
  }

  if (objects.arc_soc_1 != NULL) {
      
      float v_charger = ema_values[4]; 
      int soc_internal = get_7s_soc(v_charger);
      
      float v_gun = ema_values[5];
      int soc_gun = get_7s_soc(v_gun);

      lv_arc_set_value(objects.arc_soc_1, soc_gun);
      lv_arc_set_value(objects.arc_soc_2, soc_gun);
      lv_bar_set_value(objects.bar_internal_soc, soc_internal, LV_ANIM_ON);

      if(strlen(display_state[5]) > 0) {
          lv_label_set_text_fmt(objects.label_soc_1, "%d%%", soc_gun);
          lv_label_set_text_fmt(objects.label_soc_2, "%d%%", soc_gun);
          lv_label_set_text_fmt(objects.label_v_gun, "%s", display_state[5]);
      }

      if (is_charging && strlen(display_state[1]) > 0) {
          lv_label_set_text_fmt(objects.label_i_gun, "Charge: %s", display_state[1]);
      } else if (is_discharging && strlen(display_state[0]) > 0) {
          lv_label_set_text_fmt(objects.label_i_gun, "Discharge: %s", display_state[0]);
      } else {
          lv_label_set_text(objects.label_i_gun, "Current: 0.00 A");
      }

      if(strlen(display_state[4]) > 0) {
          lv_label_set_text_fmt(objects.label_internal_soc, "%d%%", soc_internal);
          lv_label_set_text_fmt(objects.label_internal_v, "%s", display_state[4]);
      }

      if(strlen(display_state[3]) > 0) {
          lv_label_set_text_fmt(objects.label_solar_v, "%s", display_state[3]);
      }

      if(strlen(display_state[2]) > 0) {
          lv_label_set_text_fmt(objects.label_temp, "%s", display_state[2]);
      }
  }
}