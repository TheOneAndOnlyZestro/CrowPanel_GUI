// ==========================================
// SHARED TOUCH VARIABLES
// ==========================================
int touch_last_x = 0, touch_last_y = 0;

// ==========================================
// FOR ESP32-S3 (480x272 Capacitive Touch)
// ==========================================
#ifdef TARGET_ESP32_S3

  // Uncomment the driver your S3 board originally used
  //#define TOUCH_FT6X36
  #define TOUCH_GT911

  #define TOUCH_MAP_X1 480
  #define TOUCH_MAP_X2 0
  #define TOUCH_MAP_Y1 272
  #define TOUCH_MAP_Y2 0

  #if defined(TOUCH_FT6X36)
    #include <Wire.h>
    #include <FT6X36.h>
    FT6X36 ts(&Wire, 4); // Assuming default INT pin
    bool touch_touched_flag = true, touch_released_flag = true;

  #elif defined(TOUCH_GT911)
    #include <Wire.h>
    #include <TAMC_GT911.h>
    // Adjust these to match your S3 I2C pins if they were defined externally
    #define TOUCH_GT911_SDA 19
    #define TOUCH_GT911_SCL 20
    #define TOUCH_GT911_INT -1
    #define TOUCH_GT911_RST 38
    #define TOUCH_GT911_ROTATION 0
    TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));
  #endif

  void touch_init() {
    #if defined(TOUCH_FT6X36)
      Wire.begin(19, 20);
      ts.begin();
    #elif defined(TOUCH_GT911)
      Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
      ts.begin();
      ts.setRotation(TOUCH_GT911_ROTATION);
    #endif
  }

  bool touch_has_signal() {
    #if defined(TOUCH_FT6X36)
      ts.loop();
      return touch_touched_flag || touch_released_flag;
    #elif defined(TOUCH_GT911)
      return true;
    #endif
  }

  bool touch_touched() {
    #if defined(TOUCH_FT6X36)
      if (touch_touched_flag) {
        touch_touched_flag = false;
        return true;
      }
      return false;

    #elif defined(TOUCH_GT911)
      ts.read();
      if (ts.isTouched) {
        #if defined(TOUCH_SWAP_XY)
            touch_last_x = map(ts.points[0].y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, lcd->width() - 1);
            touch_last_y = map(ts.points[0].x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, lcd->height() - 1);
        #else
            touch_last_x = map(ts.points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, lcd->width() - 1);
            touch_last_y = map(ts.points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, lcd->height() - 1);
        #endif
        return true;
      }
      return false;
    #endif
  }

  bool touch_released() {
    #if defined(TOUCH_FT6X36)
      if (touch_released_flag) {
        touch_released_flag = false;
        return true;
      }
      return false;
    #elif defined(TOUCH_GT911)
      return true;
    #endif
  }

#endif // TARGET_ESP32_S3


// ==========================================
// FOR ESP32-WROOM (ELECROW 2.8" Resistive)
// ==========================================
#ifdef TARGET_ESP32_WROOM

  #define TOUCH_XPT2046

  #define TOUCH_XPT2046_SCK  14
  #define TOUCH_XPT2046_MISO 12
  #define TOUCH_XPT2046_MOSI 13
  #define TOUCH_XPT2046_CS   33
  #define TOUCH_XPT2046_INT  36
  #define TOUCH_XPT2046_ROTATION 1 

  #define TOUCH_MAP_X1 200
  #define TOUCH_MAP_X2 4000
  #define TOUCH_MAP_Y1 200
  #define TOUCH_MAP_Y2 4000

  #include <XPT2046_Touchscreen.h>
  #include <SPI.h>

  XPT2046_Touchscreen ts(TOUCH_XPT2046_CS, TOUCH_XPT2046_INT);

  void touch_init() {
    // Uses the same SPI bus assigned in the main code
    SPI.begin(TOUCH_XPT2046_SCK, TOUCH_XPT2046_MISO, TOUCH_XPT2046_MOSI, TOUCH_XPT2046_CS);
    ts.begin();
    ts.setRotation(TOUCH_XPT2046_ROTATION);
  }

  bool touch_has_signal() {
    return ts.tirqTouched();
  }

  bool touch_touched() {
    if (ts.touched()) {
      TS_Point p = ts.getPoint();

      #if defined(TOUCH_SWAP_XY)
        touch_last_x = map(p.y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, lcd->width() - 1);
        touch_last_y = map(p.x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, lcd->height() - 1);
      #else
        touch_last_x = map(p.x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, lcd->width() - 1);
        touch_last_y = map(p.y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, lcd->height() - 1);
      #endif
      return true;
    }
    return false;
  }

  bool touch_released() {
    return true;
  }

#endif // TARGET_ESP32_WROOM