/**
 * @file hs_bar_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_bar_gen.h"
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

lv_obj_t * hs_bar_create(lv_obj_t * parent, const char * label, const char * tag, lv_subject_t * bind_value)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;
    static lv_style_t style_bar;
    static lv_style_t style_bar_360;
    static lv_style_t style_bar_240;
    static lv_style_t style_indicator;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);
        lv_style_init(&style_bar);
        lv_style_init(&style_bar_360);
        lv_style_init(&style_bar_240);
        lv_style_init(&style_indicator);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, LV_SIZE_CONTENT);
        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_base, LV_FLEX_FLOW_COLUMN);
        lv_style_set_text_color(&style_base, lv_color_hex3(0xfff));
        lv_style_set_radius(&style_base, 10);
        lv_style_set_pad_ver(&style_base, 0);
        lv_style_set_pad_row(&style_base, 10);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x171717));
        lv_style_set_bg_opa(&style_base, 25);
        lv_style_set_pad_row(&style_base_360, 6);
        lv_style_set_pad_row(&style_base_240, 4);
        lv_style_set_width(&style_bar, lv_pct(100));
        lv_style_set_bg_color(&style_bar, lv_color_hex(0x494949));
        lv_style_set_height(&style_bar, 46);
        lv_style_set_text_color(&style_bar, lv_color_hex3(0xfff));
        lv_style_set_border_width(&style_bar, 1);
        lv_style_set_border_color(&style_bar, lv_color_hex3(0xfff));
        lv_style_set_height(&style_bar_360, 36);
        lv_style_set_height(&style_bar_240, 24);
        lv_style_set_bg_color(&style_indicator, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_indicator, 255);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "hs_bar_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_obj_set_width(hs_text_normal_0, lv_pct(100));
        lv_label_set_text(hs_text_normal_0, label);
        lv_label_set_translation_tag(hs_text_normal_0, tag);

        lv_obj_t * lv_bar_0 = lv_bar_create(lv_obj_0);
        lv_bar_bind_value(lv_bar_0, bind_value);
        lv_obj_add_style(lv_bar_0, &style_bar, 0);
        lv_obj_bind_style(lv_bar_0, &style_bar_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_bar_0, &style_bar_240, 0, &sb_screen_size, 2);
        lv_obj_add_style(lv_bar_0, &style_indicator, LV_PART_INDICATOR);
        lv_obj_t * lv_label_0 = lv_label_create(lv_bar_0);
        lv_label_bind_text(lv_label_0, bind_value, "%d%%");
        lv_obj_set_align(lv_label_0, LV_ALIGN_LEFT_MID);
        lv_obj_set_x(lv_label_0, 15);
        lv_obj_set_style_blend_mode(lv_label_0, LV_BLEND_MODE_DIFFERENCE, 0);
        lv_obj_bind_style(lv_label_0, &style_text_small_466, 0, &sb_screen_size, 0);
        lv_obj_bind_style(lv_label_0, &style_text_small_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_label_0, &style_text_small_240, 0, &sb_screen_size, 2);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

