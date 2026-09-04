/**
 * @file hs_slider_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_slider_gen.h"
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

lv_obj_t * hs_slider_create(lv_obj_t * parent, const char * label, const char * tag, lv_subject_t * bind_value)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;
    static lv_style_t style_slider;
    static lv_style_t style_slider_360;
    static lv_style_t style_slider_240;
    static lv_style_t style_indicator;
    static lv_style_t style_disabled;
    static lv_style_t style_text_normal;
    static lv_style_t style_text_show;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);
        lv_style_init(&style_slider);
        lv_style_init(&style_slider_360);
        lv_style_init(&style_slider_240);
        lv_style_init(&style_indicator);
        lv_style_init(&style_disabled);
        lv_style_init(&style_text_normal);
        lv_style_init(&style_text_show);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, LV_SIZE_CONTENT);
        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_base, LV_FLEX_FLOW_COLUMN);
        lv_style_set_text_color(&style_base, lv_color_hex3(0xfff));
        lv_style_set_radius(&style_base, 10);
        lv_style_set_pad_ver(&style_base, 0);
        lv_style_set_pad_row(&style_base, 10);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x171717));
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_pad_row(&style_base_360, 6);
        lv_style_set_pad_row(&style_base_240, 4);
        lv_style_set_width(&style_slider, lv_pct(100));
        lv_style_set_bg_color(&style_slider, lv_color_hex3(0xfff));
        lv_style_set_height(&style_slider, 46);
        lv_style_set_text_color(&style_slider, lv_color_hex3(0xfff));
        lv_style_set_height(&style_slider_360, 36);
        lv_style_set_height(&style_slider_240, 24);
        lv_style_set_bg_color(&style_indicator, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_indicator, 255);
        lv_style_set_bg_color(&style_disabled, lv_color_hex(0xa3a3a3));
        lv_style_set_blend_mode(&style_text_normal, LV_BLEND_MODE_MULTIPLY);
        lv_style_set_blend_mode(&style_text_show, LV_BLEND_MODE_DIFFERENCE);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "hs_slider_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_STATE_TRICKLE, true);

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_obj_set_width(hs_text_normal_0, lv_pct(100));
        lv_label_set_text(hs_text_normal_0, label);
        lv_label_set_translation_tag(hs_text_normal_0, tag);

        lv_obj_t * lv_slider_0 = lv_slider_create(lv_obj_0);
        lv_slider_bind_value(lv_slider_0, bind_value);
        lv_obj_set_flag(lv_slider_0, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_set_flag(lv_slider_0, LV_OBJ_FLAG_STATE_TRICKLE, true);
        lv_obj_add_style(lv_slider_0, &style_slider, 0);
        lv_obj_bind_style(lv_slider_0, &style_slider_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_slider_0, &style_slider_240, 0, &sb_screen_size, 2);
        lv_obj_remove_style(lv_slider_0, NULL, LV_PART_KNOB);
        lv_obj_add_style(lv_slider_0, &style_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_slider_0, &style_disabled, LV_PART_INDICATOR | LV_STATE_DISABLED);
        lv_obj_t * hs_text_small_0 = hs_text_small_create(lv_slider_0);
        lv_label_bind_text(hs_text_small_0, bind_value, "%d%%");
        lv_obj_set_align(hs_text_small_0, LV_ALIGN_LEFT_MID);
        lv_obj_set_x(hs_text_small_0, 15);
        lv_obj_add_style(hs_text_small_0, &style_text_show, 0);
        lv_obj_add_style(hs_text_small_0, &style_text_normal, LV_STATE_DISABLED);

        lv_obj_t * hs_text_small_1 = hs_text_small_create(lv_slider_0);
        lv_obj_set_align(hs_text_small_1, LV_ALIGN_CENTER);
        lv_label_set_text(hs_text_small_1, "Not supported");
        lv_obj_add_style(hs_text_small_1, &style_text_normal, 0);
        lv_obj_add_style(hs_text_small_1, &style_text_show, LV_STATE_DISABLED);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

