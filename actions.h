#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- Expose State to main.ino ---
extern bool is_charging;
extern bool is_discharging;

// --- Actions ---
void action_charging(lv_event_t *e);
void action_power(lv_event_t *e);
void action_discharging(lv_event_t *e);
void time_label_event_cb(lv_event_t * e);

#ifdef __cplusplus
}
#endif
#endif