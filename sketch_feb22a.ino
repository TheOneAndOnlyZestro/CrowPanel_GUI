#include <lvgl.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include "ui.h"
#include "time.h"

//4.3
#define SD_MOSI 11
#define SD_MISO 13
#define SD_SCK 12
#define SD_CS 10

// #define I2S_DOUT      20
// #define I2S_BCLK      35
// #define I2S_LRC       19
// #define BUTTON_PIN    38


/******************************************************************************/

#define TFT_BL 2
#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin


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

// Your existing clock_update_cb stays almost the same
static void clock_update_cb(lv_timer_t * timer) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    char buf[16];
    strftime(buf, sizeof(buf), "%H:%M", &timeinfo);  // or "%H:%M:%S"

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
// option 1:
// ILI6485 LCD 480x272
Arduino_RPi_DPI_RGBPanel *lcd = new Arduino_RPi_DPI_RGBPanel(
  bus,
  480 /* width */, 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 43 /* hsync_back_porch */,
  272 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 12 /* vsync_back_porch */,
  1 /* pclk_active_neg */, 7000000 /* prefer_speed */, true /* auto_flush */);


#endif /* !defined(DISPLAY_DEV_KIT) */
#include "touch.h"

/*******************************************************************************
   Please config the touch panel in touch.h
 ******************************************************************************/


/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[480 * 272 / 8];
static lv_disp_drv_t disp_drv;

/* Display flushing */
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

      /*Set the coordinates*/
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
void setup()
{
  Serial.begin( 115200 ); 

  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(0, OUTPUT);//TOUCH-CS


  //lvgl初始化
  lv_init();

  // Init Display
  lcd->begin();
 
  lcd->setTextSize(2);
  lcd->fillScreen(BLACK);

  //触摸初始化
  touch_init();
  //setTouch(calData);
  screenWidth = lcd->width();
  screenHeight = lcd->height();
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 8);

  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;

  disp_drv.sw_rotate = 1;                    // crucial
  disp_drv.rotated   = LV_DISP_ROT_90;       // try 90 first; if wrong direction use LV_DISP_ROT_270

  disp_drv.flush_cb  = my_disp_flush;
  disp_drv.draw_buf  = &draw_buf;

  lv_disp_drv_register(&disp_drv);

  /* Initialize the (dummy) input device driver */
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
  static int lineIndex = 0;
  while (testingSerial.available() > 0) {
    char c = testingSerial.read();

    if (c == '\n') {
      // Store the completed line
      buffer.trim();                    // remove any trailing whitespace
      if (buffer.length() > 0) {
        strncpy(display_state[lineIndex], buffer.c_str(), 31);
        display_state[lineIndex][31] = '\0';
        
        // Debug output – very helpful right now
        Serial.print("→ slot ");
        Serial.print(lineIndex);
        Serial.print(": \"");
        Serial.print(display_state[lineIndex]);
        Serial.println("\"");
      }
      
      buffer = "";
    }
    else if (c >= 'A' && c <='J')
    {
      lineIndex = c - 'A';
    }
    else if (c != '\r' && c != 0) {
      buffer += c;
      if (buffer.length() > 40) buffer = buffer.substring(buffer.length() - 40); // prevent overflow
    }
  }

  // ==========================================
  // ASSIGN TEXT TO LVGL OBJECTS
  // ==========================================
  
  // 0 & 1 → Direct Labels (these stay the same - they are normal labels)
lv_label_set_text(objects.temp_label, display_state[0]);
lv_label_set_text(objects.ambience_label, display_state[1]);

// ================================================
// TOP PANEL (button matrix)  → indices 2 & 3
// ================================================
{
    static const char * top_map[3] = { NULL };  // 2 buttons + terminator

    top_map[0] = display_state[2];   // Voltage
    top_map[1] = display_state[3];   // Ampere
    top_map[2] = NULL;

    lv_btnmatrix_set_map(objects.top_panel, top_map);
    // Optional: force visual refresh (usually not needed)
    // lv_obj_invalidate(objects.top_panel);
}

// ================================================
// MIDDLE PANEL (button matrix) → indices 4 & 5
// ================================================
{
    static const char * mid_map[3] = { NULL };

    mid_map[0] = display_state[4];   // Battery voltage
    mid_map[1] = display_state[5];   // State of Charge
    mid_map[2] = NULL;

    lv_btnmatrix_set_map(objects.middle_panel, mid_map);
}

// ================================================
// LOWER PANEL (button matrix) → indices 6,7,8,9 (4 buttons)
// ================================================
{
    static const char * low_map[5] = { NULL };  // 4 buttons + NULL terminator

    low_map[0] = display_state[6];   // Time left
    low_map[1] = display_state[7];   // Voltage (another)
    low_map[2] = display_state[8];   // Ampere (another)
    low_map[3] = display_state[9];   // State of Charge (another)
    low_map[4] = NULL;

    lv_btnmatrix_set_map(objects.lower_panel, low_map);
}
}