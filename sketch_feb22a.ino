#include <lvgl.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include "ui.h"
#include "time.h"
#include "actions.h" // <--- ADD THIS EXACT LINE HERE
//4.3
#define SD_MOSI 11
#define SD_MISO 13
#define SD_SCK 12
#define SD_CS 10

/******************************************************************************/

#define TFT_BL 2
#define GFX_BL DF_GFX_BL 

#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *lcd = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

#define RX_PIN 18
#define TX_PIN 17

char display_state[10][32];

HardwareSerial testingSerial(1);

void set_manual_time(int year, int month, int day, int hour, int minute, int second) {
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;   // years since 1900
    timeinfo.tm_mon  = month - 1;     // 0-11
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min  = minute;
    timeinfo.tm_sec  = second;

    time_t now = mktime(&timeinfo);
    struct timeval tv = { .tv_sec = now };
    settimeofday(&tv, NULL);  // set system time
}

static void clock_update_cb(lv_timer_t * timer) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    char buf[16];
    strftime(buf, sizeof(buf), "%H:%M", &timeinfo);  

    lv_label_set_text(objects.ui_label_time, buf);
}

//UI
int led;
SPIClass& spi = SPI;

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
  GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
  40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
  45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
  5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
  8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */
);

Arduino_RPi_DPI_RGBPanel *lcd = new Arduino_RPi_DPI_RGBPanel(
  bus,
  480 /* width */, 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 43 /* hsync_back_porch */,
  272 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 12 /* vsync_back_porch */,
  1 /* pclk_active_neg */, 7000000 /* prefer_speed */, true /* auto_flush */);

#endif /* !defined(DISPLAY_DEV_KIT) */
#include "touch.h"

static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[480 * 272 / 8];
static lv_disp_drv_t disp_drv;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  lcd->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  lcd->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
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

// --- Time Editor Popup ---
lv_obj_t * time_popup = NULL;
lv_obj_t * roller_h;
lv_obj_t * roller_m;

static void save_time_cb(lv_event_t * e) {
    uint16_t h = lv_roller_get_selected(roller_h);
    uint16_t m = lv_roller_get_selected(roller_m);
    
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    set_manual_time(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, h, m, 0);
    
    lv_obj_del(time_popup);
    time_popup = NULL;
}

static void close_popup_cb(lv_event_t * e) {
    lv_obj_del(time_popup);
    time_popup = NULL;
}

extern "C" void time_label_event_cb(lv_event_t * e)  {
    if(time_popup != NULL) return; 
    
    time_popup = lv_obj_create(lv_scr_act());
    lv_obj_set_size(time_popup, 250, 200); 
    lv_obj_center(time_popup);
    
    roller_h = lv_roller_create(time_popup);
    lv_roller_set_options(roller_h, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(roller_h, 75, 100);
    lv_obj_align(roller_h, LV_ALIGN_TOP_LEFT, 15, 5); 
    
    roller_m = lv_roller_create(time_popup);
    
    static char mins_str[256] = {0}; 
    if(mins_str[0] == '\0') {
        for(int i=0; i<60; i++) sprintf(mins_str + strlen(mins_str), "%02d\n", i);
        mins_str[strlen(mins_str)-1] = '\0'; 
    }
    lv_roller_set_options(roller_m, mins_str, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(roller_m, 75, 100);
    lv_obj_align(roller_m, LV_ALIGN_TOP_RIGHT, -15, 5); 
    
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    lv_roller_set_selected(roller_h, timeinfo.tm_hour, LV_ANIM_OFF);
    lv_roller_set_selected(roller_m, timeinfo.tm_min, LV_ANIM_OFF);
    
    lv_obj_t * btn_save = lv_btn_create(time_popup);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_LEFT, 5, -5); 
    lv_obj_set_style_pad_all(btn_save, 12, LV_PART_MAIN); 
    lv_obj_add_event_cb(btn_save, save_time_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, "Save");
    
    lv_obj_t * btn_close = lv_btn_create(time_popup);
    lv_obj_align(btn_close, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
    lv_obj_set_style_pad_all(btn_close, 12, LV_PART_MAIN); 
    lv_obj_add_event_cb(btn_close, close_popup_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * lbl_close = lv_label_create(btn_close);
    lv_label_set_text(lbl_close, "Cancel");
}

void setup()
{
  Serial.begin( 115200 ); 

  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(0, OUTPUT);//TOUCH-CS

  lv_init();

  lcd->begin();
  lcd->setTextSize(2);
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
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif

  ui_init();
  lv_timer_handler();

  set_manual_time(2026, 2, 23, 16, 9, 0);
  lv_timer_create(clock_update_cb, 1000, NULL);
  clock_update_cb(NULL);

  testingSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop()
{
  lv_timer_handler();
  lv_tick_inc(5);
  delay(5);

  static String buffer = "";
  
  // --- EMA FILTER VARIABLES ---
  static float ema_values[6] = {0};
  static bool first_reading[6] = {true, true, true, true, true, true};
  const float EMA_BETA = 2.0f / (10.0f + 1.0f); // ~0.0198 for n=100

  // 1. --- ROBUST UART PARSER WITH EMA ---
  while (testingSerial.available() > 0) {
    char c = testingSerial.read();

    if (c == '\n') {
      buffer.trim();
      if (buffer.length() > 0) {
        char prefix = buffer[0]; 
        
        if (prefix >= 'A' && prefix <= 'F') {
          int index = prefix - 'A';
          
          // Extract raw float from string
          String valueStr = buffer.substring(1);
          valueStr.trim();
          float new_value = parse_float_robust(valueStr.c_str());
          
          // Apply your smoothing formula: (1 - beta) * acc + beta * new_value
          if (first_reading[index]) {
              ema_values[index] = new_value; // Snap to first reading instantly
              first_reading[index] = false;
          } else {
              ema_values[index] = (1.0f - EMA_BETA) * ema_values[index] + (EMA_BETA * new_value);
          }

          // Format back to string with appropriate units
          char tempStr[32];
          if (index == 0 || index == 1) { // Amps
              snprintf(tempStr, sizeof(tempStr), "%.2f A", ema_values[index]);
          } else if (index == 2) { // Temp
              snprintf(tempStr, sizeof(tempStr), "%.1f C", ema_values[index]);
          } else { // Voltage
              snprintf(tempStr, sizeof(tempStr), "%.2f V", ema_values[index]);
          }

          // Save the smoothed string for the UI to display
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

  // 2. --- CALCULATE SOC (Using smoothed values) ---
  float v_charger = ema_values[4]; // Use the numerical array directly for math
  int soc_top = get_7s_soc(v_charger);
  
  float v_gun = ema_values[5];
  int soc_low = get_7s_soc(v_gun);

  lv_bar_set_value(objects.charging_bar, soc_top, LV_ANIM_ON);

  if(strlen(display_state[2]) > 0) {
      lv_label_set_text_fmt(objects.temp_label, "%s", display_state[2]);
  }

  // 3. --- TOP PANEL UPDATE ---
  static char top_v_solar[32] = "Solar: --";
  static char top_v_charger[32] = "Charger: --";
  static char top_soc_str[32] = "SOC: --";
  
  if(strlen(display_state[3]) > 0) snprintf(top_v_solar, sizeof(top_v_solar), "Solar: %s", display_state[3]);
  if(strlen(display_state[4]) > 0) snprintf(top_v_charger, sizeof(top_v_charger), "Charger: %s", display_state[4]);
  snprintf(top_soc_str, sizeof(top_soc_str), "SOC: %d%%", soc_top);

  static const char * top_map[7] = { top_v_solar, "\n", top_v_charger, "\n", top_soc_str, "", NULL };
  lv_btnmatrix_set_map(objects.top_panel, top_map);
  lv_obj_invalidate(objects.top_panel); 

  // 4. --- LOWER PANEL UPDATE ---
  static char low_v_gun[32] = "Gun: --";
  static char low_current[32] = "Current: --";
  static char low_soc_str[32] = "SOC: --";
  
  if(strlen(display_state[5]) > 0) snprintf(low_v_gun, sizeof(low_v_gun), "Gun: %s", display_state[5]);
  
  if (is_charging && strlen(display_state[1]) > 0) {
      snprintf(low_current, sizeof(low_current), "Charging: %s", display_state[1]);
  } else if (is_discharging && strlen(display_state[0]) > 0) {
      snprintf(low_current, sizeof(low_current), "Discharging: %s", display_state[0]);
  } else {
      snprintf(low_current, sizeof(low_current), "Current: 0.00 A");
  }

  snprintf(low_soc_str, sizeof(low_soc_str), "SOC: %d%%", soc_low);

  static const char * low_map[7] = { low_v_gun, "\n", low_current, "\n", low_soc_str, "", NULL };
  lv_btnmatrix_set_map(objects.lower_panel, low_map);
  lv_obj_invalidate(objects.lower_panel);
}