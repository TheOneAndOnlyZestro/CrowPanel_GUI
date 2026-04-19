#include "config.h"

#include "actions.h"
#include "screens.h" 
#include <Arduino.h>
#include <lvgl.h>

#define COLOR_ON  lv_color_hex(0x00E676) // Bright Green
#define COLOR_OFF lv_color_hex(0xF44336) // Red

bool is_charging = false;
bool is_discharging = false;

extern "C" void action_brightness(lv_event_t * e) {
    int value = (int)lv_slider_get_value(lv_event_get_target(e));
    int pwmValue = map(value, 0, 100, 0, 255);
    ledcWrite(0, pwmValue); 
}

extern "C" void action_contrast(lv_event_t * e) {
    int value = (int)lv_slider_get_value(lv_event_get_target(e));
    Serial.printf("Contrast adjusting to %d\n", value);
}

extern "C" void action_hue(lv_event_t * e) {
    int hue_val = (int)lv_slider_get_value(lv_event_get_target(e));
    lv_color_t custom_primary = lv_color_hsv_to_rgb(hue_val, 80, 100);
    lv_disp_t * disp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(disp, custom_primary, lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(disp, theme);
}

extern "C" void action_theme_switch(lv_event_t * e) {}

// --- CHARGING GAUGE TOGGLE ---
extern "C" void action_charging(lv_event_t * e) {
    if (is_discharging) return; 

    pinMode(CHARGE_CONTROL_PIN, OUTPUT);
    is_charging = !is_charging; 

    if (is_charging) {
        // Paint it Green
        lv_obj_set_style_arc_color(objects.arc_soc_1, COLOR_ON, LV_PART_INDICATOR); 
        lv_obj_set_style_bg_color(objects.bg_circle_1, COLOR_ON, 0); 
        // Darken text for readability on green
        lv_obj_set_style_text_color(objects.label_soc_1, lv_color_hex(0x121212), 0);
        lv_obj_set_style_text_color(objects.label_status_1, lv_color_hex(0x121212), 0);
        lv_label_set_text(objects.label_status_1, "STARTED");
        
        digitalWrite(CHARGE_CONTROL_PIN, HIGH); 
    } else {
        // Revert to Red
        lv_obj_set_style_arc_color(objects.arc_soc_1, COLOR_OFF, LV_PART_INDICATOR); 
        lv_obj_set_style_bg_color(objects.bg_circle_1, COLOR_OFF, 0); 
        // Lighten text for readability on red
        lv_obj_set_style_text_color(objects.label_soc_1, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_color(objects.label_status_1, lv_color_hex(0xFFFFFF), 0);
        lv_label_set_text(objects.label_status_1, "STOPPED");
        
        digitalWrite(CHARGE_CONTROL_PIN, LOW);
    }
}

// --- DISCHARGING GAUGE TOGGLE ---
extern "C" void action_discharging(lv_event_t * e) {
    if (is_charging) return; 

    pinMode(DISCHARGE_CONTROL_PIN, OUTPUT);
    is_discharging = !is_discharging; 

    if (is_discharging) {
        lv_obj_set_style_arc_color(objects.arc_soc_2, COLOR_ON, LV_PART_INDICATOR); 
        lv_obj_set_style_bg_color(objects.bg_circle_2, COLOR_ON, 0); 
        lv_obj_set_style_text_color(objects.label_soc_2, lv_color_hex(0x121212), 0);
        lv_obj_set_style_text_color(objects.label_status_2, lv_color_hex(0x121212), 0);
        lv_label_set_text(objects.label_status_2, "STARTED");
        
        digitalWrite(DISCHARGE_CONTROL_PIN, HIGH); 
    } else {
        lv_obj_set_style_arc_color(objects.arc_soc_2, COLOR_OFF, LV_PART_INDICATOR); 
        lv_obj_set_style_bg_color(objects.bg_circle_2, COLOR_OFF, 0); 
        lv_obj_set_style_text_color(objects.label_soc_2, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_color(objects.label_status_2, lv_color_hex(0xFFFFFF), 0);
        lv_label_set_text(objects.label_status_2, "STOPPED");
        
        digitalWrite(DISCHARGE_CONTROL_PIN, LOW);
    } 
}