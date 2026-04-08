#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *splash_scr;
    lv_obj_t *main_scr;
    
    lv_obj_t *tabview;
    
    // Page 1 (Charge)
    lv_obj_t *arc_soc_1;
    lv_obj_t *bg_circle_1;
    lv_obj_t *label_soc_1;
    lv_obj_t *label_status_1;
    lv_obj_t *label_v_gun;
    lv_obj_t *label_i_gun;
    lv_obj_t *bar_internal_soc;
    lv_obj_t *label_internal_soc;

    // Page 2 (Discharge/Advanced)
    lv_obj_t *arc_soc_2;
    lv_obj_t *bg_circle_2;
    lv_obj_t *label_soc_2;
    lv_obj_t *label_status_2;
    lv_obj_t *label_solar_v;
    lv_obj_t *label_temp;
    lv_obj_t *label_internal_v;

    // Page 3 (Settings)
    lv_obj_t *slider_brightness;
    lv_obj_t *slider_contrast;
    lv_obj_t *slider_hue;
    lv_obj_t *switch_theme;
} objects_t;

extern objects_t objects;

void create_screen_main();
void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/