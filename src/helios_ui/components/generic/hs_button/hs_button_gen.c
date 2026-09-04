/**
 * @file hs_button_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_button_gen.h"
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

lv_obj_t * hs_button_create(lv_obj_t * parent, const char * label, const char * tag, lv_color_t text_color)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;
    static lv_style_t style_pressed;
    static lv_style_t style_checked;
    static lv_style_t style_checked_text;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);
        lv_style_init(&style_pressed);
        lv_style_init(&style_checked);
        lv_style_init(&style_checked_text);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, LV_SIZE_CONTENT);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x191919));
        lv_style_set_radius(&style_base, 10);
        lv_style_set_pad_all(&style_base, 10);
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_pad_all(&style_base_360, 6);
        lv_style_set_radius(&style_base_360, 8);
        lv_style_set_pad_all(&style_base_240, 4);
        lv_style_set_radius(&style_base_240, 6);
        lv_style_set_bg_color(&style_pressed, lv_color_hex(0xbcbcbc));
        lv_style_set_bg_opa(&style_pressed, 155);
        lv_style_set_bg_color(&style_checked, lv_color_hex(0xf9f7f7));
        lv_style_set_bg_opa(&style_checked, 155);
        lv_style_set_blend_mode(&style_checked_text, LV_BLEND_MODE_DIFFERENCE);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "hs_button_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_STATE_TRICKLE, true);

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_add_style(lv_obj_0, &style_pressed, LV_STATE_PRESSED);
        lv_obj_add_style(lv_obj_0, &style_checked, LV_STATE_CHECKED);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_label_set_text(hs_text_normal_0, label);
        lv_label_set_translation_tag(hs_text_normal_0, tag);
        lv_obj_set_width(hs_text_normal_0, lv_pct(100));
        lv_obj_set_style_pad_top(hs_text_normal_0, 5, 0);
        lv_obj_set_style_text_color(hs_text_normal_0, text_color, 0);
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_add_style(hs_text_normal_0, &style_checked_text, LV_STATE_CHECKED);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

