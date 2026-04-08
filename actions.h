#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool is_charging;
extern bool is_discharging;

void action_charging(lv_event_t *e);
void action_discharging(lv_event_t *e);
void action_brightness(lv_event_t *e);
void action_contrast(lv_event_t *e);
void action_hue(lv_event_t *e);
void action_theme_switch(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif