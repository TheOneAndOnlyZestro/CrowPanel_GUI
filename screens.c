#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

objects_t objects;

// Event handlers
lv_obj_t *tick_value_change_obj;

// Screens
void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 272, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            // start_button
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.start_button = obj;
            lv_obj_set_pos(obj, 26, 416);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_charging, LV_EVENT_CLICKED, (void *)0);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x2196F3), LV_PART_MAIN); 
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Start Charge");
                }
            }
        }
        {
            // charging_bar
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.charging_bar = obj;
            lv_obj_set_pos(obj, 16, 397);
            lv_obj_set_size(obj, 240, 10);
            lv_bar_set_value(obj, 0, LV_ANIM_ON);
        }
        {
            // power_btn
            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
            objects.power_btn = obj;
            lv_obj_set_pos(obj, 103, 6);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, 60);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_p, NULL);
            lv_obj_add_event_cb(obj, action_power, LV_EVENT_CLICKED, (void *)0);
        }
        {
            // main_panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.main_panel = obj;
            lv_obj_set_pos(obj, 0, 74);
            lv_obj_set_size(obj, 272, 312);
            {
                lv_obj_t *parent_obj = obj;
                
                {
                    // temp_label (Perfectly centered, 5px from top edge)
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.temp_label = obj;
                    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 5); 
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN);
                    lv_label_set_text(obj, "24 C");
                }
                {
                    // "Charge Metrics" Title (Centered, pushed down to 45px)
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 45); 
                    lv_label_set_text(obj, "Charge Metrics");
                }
                {
                    // top_panel (Pushed down to 65px)
                    lv_obj_t *obj = lv_btnmatrix_create(parent_obj);
                    objects.top_panel = obj;
                    lv_obj_set_pos(obj, -2, 65);
                    lv_obj_set_size(obj, 240, 100);
                    static const char *map[] = { "Voltage Solar", "\n", "Voltage Charger", "\n", "Soc. Batt", "" };
                    lv_btnmatrix_set_map(obj, map);
                }
                {
                    // "Gun Metrics" Title (Centered, pushed down to 175px)
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 175);
                    lv_label_set_text(obj, "Gun Metrics");
                }
                {
                    // lower_panel (Pushed down to 195px)
                    lv_obj_t *obj = lv_btnmatrix_create(parent_obj);
                    objects.lower_panel = obj;
                    lv_obj_set_pos(obj, -2, 195);
                    lv_obj_set_size(obj, 240, 100);
                    static const char *map[] = { "Voltage Gun", "\n", "Current", "\n", "SOC", "" };
                    lv_btnmatrix_set_map(obj, map);
                }
            }
        }
        {
            // ui_label_time
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.ui_label_time = obj;
            lv_obj_set_pos(obj, 13, 17);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "13:30");
            
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE); 
            lv_obj_add_event_cb(obj, time_label_event_cb, LV_EVENT_CLICKED, NULL);
        }
        {
            // logo_img
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.logo_img = obj;
            lv_obj_set_pos(obj, 163, 10);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_logo);
        }
        {
            // start_button_d
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.start_button_d = obj;
            lv_obj_set_pos(obj, 144, 416);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, action_discharging, LV_EVENT_CLICKED, (void *)0);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x2196F3), LV_PART_MAIN);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Start Dischrg");
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

ext_font_desc_t fonts[] = {
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

uint32_t active_theme_index = 0;

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}