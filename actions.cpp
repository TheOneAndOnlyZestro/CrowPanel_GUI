#include "actions.h"
#include <Arduino.h>
#include <lvgl.h>

// --- PIN MAPPING ---
// Note: Double check if pin 38 is your backlight. If it is, you might want to use a different pin for charging!
const int CHARGE_CONTROL_PIN = 38;  
const int DISCHARGE_CONTROL_PIN = 37;  

// --- GLOBAL STATE ---
bool is_charging = false;
bool is_discharging = false;

extern "C" void action_charging(lv_event_t * e) {
    // 1. Mutually Exclusive Check
    if (is_discharging) {
        Serial.println("Action Denied: Cannot start charging while discharging!");
        return; 
    }

    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);

    pinMode(CHARGE_CONTROL_PIN, OUTPUT);

    // 2. Toggle State
    is_charging = !is_charging; 

    // 3. Apply UI and Hardware Changes
    if (is_charging) {
        // UI Updates
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x4CAF50), LV_PART_MAIN); // Green
        lv_label_set_text(label, "Stop Charge");
        
        // Hardware Updates
        digitalWrite(CHARGE_CONTROL_PIN, HIGH); 
        Serial.println("GPIO 38: HIGH (Charging Started)");
    } else {
        // UI Updates
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2196F3), LV_PART_MAIN); // Default Blue
        lv_label_set_text(label, "Start Charge");
        
        // Hardware Updates
        digitalWrite(CHARGE_CONTROL_PIN, LOW);
        Serial.println("GPIO 38: LOW (Charging Stopped)");
    }
}

extern "C" void action_discharging(lv_event_t * e) {
    // 1. Mutually Exclusive Check
    if (is_charging) {
        Serial.println("Action Denied: Cannot start discharging while charging!");
        return; 
    }

    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);

    pinMode(DISCHARGE_CONTROL_PIN, OUTPUT);

    // 2. Toggle State
    is_discharging = !is_discharging; 

    // 3. Apply UI and Hardware Changes
    if (is_discharging) {
        // UI Updates
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF9800), LV_PART_MAIN); // Orange
        lv_label_set_text(label, "Stop Dischrg");
        
        // Hardware Updates
        digitalWrite(DISCHARGE_CONTROL_PIN, HIGH); 
        Serial.println("GPIO 37: HIGH (Discharging Started)");
    } else {
        // UI Updates
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2196F3), LV_PART_MAIN); // Default Blue
        lv_label_set_text(label, "Start Dischrg");
        
        // Hardware Updates
        digitalWrite(DISCHARGE_CONTROL_PIN, LOW);
        Serial.println("GPIO 37: LOW (Discharging Stopped)");
    } 
}

extern "C" void action_power(lv_event_t * e) {
    Serial.println("Executing Power Down...");
    Serial.println("Backlight OFF, GPIO 2 HIGH.");
}