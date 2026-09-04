/**
 * @file hs_switch_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_switch_gen.h"
#include "../../../helios_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * hs_switch_create(lv_obj_t * parent, const char * label, const char * tag, lv_subject_t * bind_value)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;
    static lv_style_t style_switch;
    static lv_style_t style_switch_360;
    static lv_style_t style_switch_240;
    static lv_style_t style_indicator;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);
        lv_style_init(&style_switch);
        lv_style_init(&style_switch_360);
        lv_style_init(&style_switch_240);
        lv_style_init(&style_indicator);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, LV_SIZE_CONTENT);
        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_base, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_track_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_text_color(&style_base, lv_color_hex3(0xfff));
        lv_style_set_radius(&style_base, 10);
        lv_style_set_pad_ver(&style_base, 0);
        lv_style_set_pad_row(&style_base, 10);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x171717));
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_pad_ver(&style_base_360, 2);
        lv_style_set_pad_ver(&style_base_240, 4);
        lv_style_set_bg_color(&style_switch, lv_color_hex(0xffffff));
        lv_style_set_width(&style_switch, 80);
        lv_style_set_height(&style_switch, 46);
        lv_style_set_text_color(&style_switch, lv_color_hex3(0xfff));
        lv_style_set_width(&style_switch_360, 60);
        lv_style_set_height(&style_switch_360, 36);
        lv_style_set_width(&style_switch_240, 40);
        lv_style_set_height(&style_switch_240, 24);
        lv_style_set_bg_color(&style_indicator, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_indicator, 255);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "hs_switch_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_label_set_text(hs_text_normal_0, label);
        lv_label_set_translation_tag(hs_text_normal_0, tag);
        lv_obj_set_flex_grow(hs_text_normal_0, 1);
        lv_obj_set_style_pad_top(hs_text_normal_0, 5, 0);

        lv_obj_t * lv_switch_0 = lv_switch_create(lv_obj_0);
        lv_obj_bind_checked(lv_switch_0, bind_value);
        lv_obj_set_ext_click_area(lv_switch_0, 10);
        lv_obj_add_style(lv_switch_0, &style_switch, 0);
        lv_obj_add_style(lv_switch_0, &style_primary_bg, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(lv_switch_0, &style_primary_bg, LV_PART_KNOB);
        lv_obj_add_style(lv_switch_0, &style_white_bg, LV_PART_KNOB | LV_STATE_CHECKED);
        lv_obj_bind_style(lv_switch_0, &style_switch_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_switch_0, &style_switch_240, 0, &sb_screen_size, 2);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

