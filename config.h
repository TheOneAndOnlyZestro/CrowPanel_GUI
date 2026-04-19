#ifndef CONFIG_H
#define CONFIG_H

#include <lvgl.h>

// =========================================================
// SELECT YOUR HARDWARE TARGET HERE (Uncomment ONLY ONE)
// =========================================================
#define TARGET_ESP32_S3
//#define TARGET_ESP32_WROOM

// =========================================================
// HARDWARE PINS & UI DIMENSIONS
// =========================================================

#ifdef TARGET_ESP32_S3
  // --- ESP32-S3 (480x272 Screen) ---
  
  // Hardware Pins
  #define CHARGE_CONTROL_PIN     38  
  #define DISCHARGE_CONTROL_PIN  37  

  // UI Layout Sizes (Includes 4mm left bezel fix)
  #define UI_TAB_WIDTH         252
  #define UI_TAB_ALIGN_X       0
  #define UI_LOGO_OFFSET_X     -25
  #define UI_ARC_MAIN_SIZE     180
  #define UI_BG_MAIN_SIZE      140
  #define UI_ARC_ADV_SIZE      160
  #define UI_BG_ADV_SIZE       120
  #define UI_BAR_WIDTH         200
  #define UI_ADV_PANEL_W       240
  #define UI_ROW_HEIGHT        45
  #define UI_SLIDER_WIDTH      220

  // Fonts
  #define FONT_SOC_MAIN        &lv_font_montserrat_34
  #define FONT_SOC_ADV         &lv_font_montserrat_24
  #define FONT_DATA_L          &lv_font_montserrat_24
  #define FONT_DATA_M          &lv_font_montserrat_20
  #define FONT_DATA_S          &lv_font_montserrat_16
  #define FONT_DATA_XS         &lv_font_montserrat_14

#elif defined(TARGET_ESP32_WROOM)
  // --- ESP32-WROOM (320x240 Screen) ---
  
  // Hardware Pins
  #define CHARGE_CONTROL_PIN     25  
  #define DISCHARGE_CONTROL_PIN  32  

  // UI Layout Sizes (Scaled down for smaller screen)
  #define UI_TAB_WIDTH         LV_PCT(100) 
  #define UI_TAB_ALIGN_X       0
  #define UI_LOGO_OFFSET_X     -5
  #define UI_ARC_MAIN_SIZE     140
  #define UI_BG_MAIN_SIZE      100
  #define UI_ARC_ADV_SIZE      120
  #define UI_BG_ADV_SIZE       80
  #define UI_BAR_WIDTH         160
  #define UI_ADV_PANEL_W       200
  #define UI_ROW_HEIGHT        35
  #define UI_SLIDER_WIDTH      180

  // Fonts
  #define FONT_SOC_MAIN        &lv_font_montserrat_24
  #define FONT_SOC_ADV         &lv_font_montserrat_20
  #define FONT_DATA_L          &lv_font_montserrat_20
  #define FONT_DATA_M          &lv_font_montserrat_16
  #define FONT_DATA_S          &lv_font_montserrat_14
  #define FONT_DATA_XS         &lv_font_montserrat_12

#endif

#endif // CONFIG_H