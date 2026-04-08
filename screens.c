#include "screens.h"
#include "images.h"
#include "actions.h"
#include "ui.h"

objects_t objects;

#define COLOR_ON  lv_color_hex(0x00E676) // Neon Green
#define COLOR_OFF lv_color_hex(0xF44336) // Red

static void splash_timer_cb(lv_timer_t * timer) {
    lv_scr_load_anim(objects.main_scr, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
    lv_timer_del(timer);
}

void create_screen_main() {
    // ==========================================
    // 1. SPLASH SCREEN
    // ==========================================
    objects.splash_scr = lv_obj_create(NULL);
    lv_obj_t * splash_logo = lv_img_create(objects.splash_scr);
    lv_img_set_src(splash_logo, &img_logo);
    lv_obj_center(splash_logo);
    lv_timer_create(splash_timer_cb, 3000, NULL); 

    // ==========================================
    // 2. MAIN INTERFACE 
    // ==========================================
    objects.main_scr = lv_obj_create(NULL);

    // Global Mini Logo
    lv_obj_t * mini_logo = lv_img_create(objects.main_scr);
    lv_img_set_src(mini_logo, &img_logo);
    lv_img_set_zoom(mini_logo, 75); 
    lv_obj_align(mini_logo, LV_ALIGN_TOP_LEFT, 5, 5);

    // Tabview on the BOTTOM
    objects.tabview = lv_tabview_create(objects.main_scr, LV_DIR_BOTTOM, 50);
    lv_obj_t * tab_home = lv_tabview_add_tab(objects.tabview, LV_SYMBOL_HOME);
    lv_obj_t * tab_adv  = lv_tabview_add_tab(objects.tabview, LV_SYMBOL_LIST);
    lv_obj_t * tab_set  = lv_tabview_add_tab(objects.tabview, LV_SYMBOL_SETTINGS);

    lv_obj_clear_flag(objects.tabview, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(lv_tabview_get_content(objects.tabview), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(tab_home, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(tab_adv, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(tab_set, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_flow(tab_home, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tab_home, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_flex_flow(tab_adv, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tab_adv, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_flex_flow(tab_set, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tab_set, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(tab_set, 15, 0); 

    // ------------------------------------------
    // TAB 1: HOME (Charging)
    // ------------------------------------------
    objects.arc_soc_1 = lv_arc_create(tab_home);
    lv_obj_set_size(objects.arc_soc_1, 180, 180);
    lv_arc_set_rotation(objects.arc_soc_1, 135);
    lv_arc_set_bg_angles(objects.arc_soc_1, 0, 270);
    
    lv_obj_add_flag(objects.arc_soc_1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(objects.arc_soc_1, action_charging, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_arc_width(objects.arc_soc_1, 12, LV_PART_MAIN);
    lv_obj_set_style_arc_color(objects.arc_soc_1, COLOR_OFF, LV_PART_INDICATOR); 
    lv_obj_set_style_arc_width(objects.arc_soc_1, 12, LV_PART_INDICATOR);
    lv_obj_remove_style(objects.arc_soc_1, NULL, LV_PART_KNOB); 

    // INNER FILLED CIRCLE
    objects.bg_circle_1 = lv_obj_create(objects.arc_soc_1);
    lv_obj_set_size(objects.bg_circle_1, 140, 140);
    lv_obj_center(objects.bg_circle_1);
    lv_obj_set_style_radius(objects.bg_circle_1, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(objects.bg_circle_1, COLOR_OFF, 0);
    lv_obj_set_style_border_width(objects.bg_circle_1, 0, 0);
    lv_obj_clear_flag(objects.bg_circle_1, LV_OBJ_FLAG_CLICKABLE); // Let touch pass to the Arc
    lv_obj_clear_flag(objects.bg_circle_1, LV_OBJ_FLAG_SCROLLABLE);

    objects.label_soc_1 = lv_label_create(objects.bg_circle_1);
    lv_obj_align(objects.label_soc_1, LV_ALIGN_CENTER, 0, -12);
    lv_obj_set_style_text_font(objects.label_soc_1, &lv_font_montserrat_34, 0);
    lv_label_set_text(objects.label_soc_1, "--%"); 

    objects.label_status_1 = lv_label_create(objects.bg_circle_1);
    lv_obj_align(objects.label_status_1, LV_ALIGN_CENTER, 0, 25);
    lv_obj_set_style_text_font(objects.label_status_1, &lv_font_montserrat_16, 0);
    lv_label_set_text(objects.label_status_1, "STOPPED"); 

    // Data Block
    objects.label_v_gun = lv_label_create(tab_home);
    lv_obj_set_style_text_font(objects.label_v_gun, &lv_font_montserrat_24, 0);
    lv_label_set_text(objects.label_v_gun, "0.0 V (PLACEHOLDER)");

    objects.label_i_gun = lv_label_create(tab_home);
    lv_obj_set_style_text_font(objects.label_i_gun, &lv_font_montserrat_20, 0);
    lv_label_set_text(objects.label_i_gun, "0.0 A (PLACEHOLDER)");

    lv_obj_t * lbl_int = lv_label_create(tab_home);
    lv_label_set_text(lbl_int, "Internal Battery");
    lv_obj_set_style_text_font(lbl_int, &lv_font_montserrat_14, 0);

    objects.bar_internal_soc = lv_bar_create(tab_home);
    lv_obj_set_size(objects.bar_internal_soc, 200, 15);
    lv_obj_set_style_bg_color(objects.bar_internal_soc, COLOR_ON, LV_PART_INDICATOR);

    objects.label_internal_soc = lv_label_create(tab_home);
    lv_label_set_text(objects.label_internal_soc, "0% (PLACEHOLDER)");

    // ------------------------------------------
    // TAB 2: ADVANCED (Discharging & Clean Rows)
    // ------------------------------------------
    objects.arc_soc_2 = lv_arc_create(tab_adv);
    lv_obj_set_size(objects.arc_soc_2, 160, 160);
    lv_arc_set_rotation(objects.arc_soc_2, 135);
    lv_arc_set_bg_angles(objects.arc_soc_2, 0, 270);
    
    lv_obj_add_flag(objects.arc_soc_2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(objects.arc_soc_2, action_discharging, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_arc_width(objects.arc_soc_2, 12, LV_PART_MAIN);
    lv_obj_set_style_arc_color(objects.arc_soc_2, COLOR_OFF, LV_PART_INDICATOR); 
    lv_obj_set_style_arc_width(objects.arc_soc_2, 12, LV_PART_INDICATOR);
    lv_obj_remove_style(objects.arc_soc_2, NULL, LV_PART_KNOB); 

    // INNER FILLED CIRCLE 2
    objects.bg_circle_2 = lv_obj_create(objects.arc_soc_2);
    lv_obj_set_size(objects.bg_circle_2, 120, 120);
    lv_obj_center(objects.bg_circle_2);
    lv_obj_set_style_radius(objects.bg_circle_2, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(objects.bg_circle_2, COLOR_OFF, 0);
    lv_obj_set_style_border_width(objects.bg_circle_2, 0, 0);
    lv_obj_clear_flag(objects.bg_circle_2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(objects.bg_circle_2, LV_OBJ_FLAG_SCROLLABLE);

    objects.label_soc_2 = lv_label_create(objects.bg_circle_2);
    lv_obj_align(objects.label_soc_2, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_text_font(objects.label_soc_2, &lv_font_montserrat_24, 0);
    lv_label_set_text(objects.label_soc_2, "--%"); 

    objects.label_status_2 = lv_label_create(objects.bg_circle_2);
    lv_obj_align(objects.label_status_2, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_text_font(objects.label_status_2, &lv_font_montserrat_12, 0);
    lv_label_set_text(objects.label_status_2, "STOPPED"); 

    // --- ENHANCED ROWS PANEL ---
    lv_obj_t * panel_adv = lv_obj_create(tab_adv);
    lv_obj_set_size(panel_adv, 240, 160);
    lv_obj_set_style_bg_opa(panel_adv, 0, 0);
    lv_obj_set_style_border_width(panel_adv, 0, 0);
    lv_obj_set_style_pad_all(panel_adv, 0, 0);
    lv_obj_set_flex_flow(panel_adv, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(panel_adv, LV_OBJ_FLAG_SCROLLABLE);

    // Row 1: Solar (Yellow)
    lv_obj_t * row1 = lv_obj_create(panel_adv);
    lv_obj_set_size(row1, LV_PCT(100), 45);
    lv_obj_set_style_bg_opa(row1, 0, 0);
    lv_obj_set_style_border_width(row1, 0, 0);
    lv_obj_set_style_border_width(row1, 2, LV_PART_MAIN); 
    lv_obj_set_style_border_side(row1, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(row1, lv_color_hex(0x444444), 0);
    lv_obj_clear_flag(row1, LV_OBJ_FLAG_SCROLLABLE);
    
    lv_obj_t * title1 = lv_label_create(row1);
    lv_label_set_text(title1, "Solar");
    lv_obj_align(title1, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_text_font(title1, &lv_font_montserrat_16, 0);

    objects.label_solar_v = lv_label_create(row1);
    lv_label_set_text(objects.label_solar_v, "(PLACEHOLDER)");
    lv_obj_align(objects.label_solar_v, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_text_font(objects.label_solar_v, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(objects.label_solar_v, lv_color_hex(0xFFEB3B), 0); 

    // Row 2: Temp (Cyan)
    lv_obj_t * row2 = lv_obj_create(panel_adv);
    lv_obj_set_size(row2, LV_PCT(100), 45);
    lv_obj_set_style_bg_opa(row2, 0, 0);
    lv_obj_set_style_border_width(row2, 0, 0);
    lv_obj_set_style_border_width(row2, 2, LV_PART_MAIN);
    lv_obj_set_style_border_side(row2, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(row2, lv_color_hex(0x444444), 0);
    lv_obj_clear_flag(row2, LV_OBJ_FLAG_SCROLLABLE);
    
    lv_obj_t * title2 = lv_label_create(row2);
    lv_label_set_text(title2, "Temp");
    lv_obj_align(title2, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_text_font(title2, &lv_font_montserrat_16, 0);

    objects.label_temp = lv_label_create(row2);
    lv_label_set_text(objects.label_temp, "(PLACEHOLDER)");
    lv_obj_align(objects.label_temp, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_text_font(objects.label_temp, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(objects.label_temp, lv_color_hex(0x00E5FF), 0); 

    // Row 3: Internal Bat (Purple)
    lv_obj_t * row3 = lv_obj_create(panel_adv);
    lv_obj_set_size(row3, LV_PCT(100), 45);
    lv_obj_set_style_bg_opa(row3, 0, 0);
    lv_obj_set_style_border_width(row3, 0, 0);
    lv_obj_set_style_border_width(row3, 2, LV_PART_MAIN);
    lv_obj_set_style_border_side(row3, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(row3, lv_color_hex(0x444444), 0);
    lv_obj_clear_flag(row3, LV_OBJ_FLAG_SCROLLABLE);
    
    lv_obj_t * title3 = lv_label_create(row3);
    lv_label_set_text(title3, "Internal");
    lv_obj_align(title3, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_text_font(title3, &lv_font_montserrat_16, 0);

    objects.label_internal_v = lv_label_create(row3);
    lv_label_set_text(objects.label_internal_v, "(PLACEHOLDER)");
    lv_obj_align(objects.label_internal_v, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_text_font(objects.label_internal_v, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(objects.label_internal_v, lv_color_hex(0xB388FF), 0); 

    // ------------------------------------------
    // TAB 3: SETTINGS
    // ------------------------------------------
    lv_obj_t * spacer = lv_obj_create(tab_set);
    lv_obj_set_size(spacer, 10, 20);
    lv_obj_set_style_bg_opa(spacer, 0, 0);
    lv_obj_set_style_border_width(spacer, 0, 0);

    lv_obj_t * lbl_b = lv_label_create(tab_set);
    lv_label_set_text(lbl_b, "Screen Brightness");
    objects.slider_brightness = lv_slider_create(tab_set);
    lv_obj_set_width(objects.slider_brightness, 220); 
    lv_slider_set_range(objects.slider_brightness, 5, 100);
    lv_slider_set_value(objects.slider_brightness, 100, LV_ANIM_OFF);
    lv_obj_clear_flag(objects.slider_brightness, LV_OBJ_FLAG_SCROLL_CHAIN); 
    lv_obj_add_event_cb(objects.slider_brightness, action_brightness, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t * lbl_c = lv_label_create(tab_set);
    lv_label_set_text(lbl_c, "Contrast (SPI Req)");
    objects.slider_contrast = lv_slider_create(tab_set);
    lv_obj_set_width(objects.slider_contrast, 220);
    lv_slider_set_range(objects.slider_contrast, 0, 100);
    lv_slider_set_value(objects.slider_contrast, 50, LV_ANIM_OFF);
    lv_obj_clear_flag(objects.slider_contrast, LV_OBJ_FLAG_SCROLL_CHAIN); 
    lv_obj_add_event_cb(objects.slider_contrast, action_contrast, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t * lbl_h = lv_label_create(tab_set);
    lv_label_set_text(lbl_h, "Theme Accent Hue");
    objects.slider_hue = lv_slider_create(tab_set);
    lv_obj_set_width(objects.slider_hue, 220);
    lv_slider_set_range(objects.slider_hue, 0, 360); 
    lv_slider_set_value(objects.slider_hue, 200, LV_ANIM_OFF); 
    lv_obj_clear_flag(objects.slider_hue, LV_OBJ_FLAG_SCROLL_CHAIN); 
    lv_obj_add_event_cb(objects.slider_hue, action_hue, LV_EVENT_VALUE_CHANGED, NULL);
}

void create_screens() {
    create_screen_main();
    lv_scr_load(objects.splash_scr); 
}