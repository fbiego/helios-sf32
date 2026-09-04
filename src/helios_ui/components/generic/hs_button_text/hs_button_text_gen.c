/**
 * @file hs_button_text_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_button_text_gen.h"
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

lv_obj_t * hs_button_text_create(lv_obj_t * parent, const char * text, const char * tag, int32_t rotation)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_button;
    static lv_style_t style_button_360;
    static lv_style_t style_button_240;
    static lv_style_t style_pressed;
    static lv_style_t style_icon;
    static lv_style_t style_checked;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_button);
        lv_style_init(&style_button_360);
        lv_style_init(&style_button_240);
        lv_style_init(&style_pressed);
        lv_style_init(&style_icon);
        lv_style_init(&style_checked);

        lv_style_set_width(&style_button, LV_SIZE_CONTENT);
        lv_style_set_height(&style_button, 50);
        lv_style_set_radius(&style_button, 55);
        lv_style_set_bg_color(&style_button, lv_color_hex(0x282828));
        lv_style_set_bg_opa(&style_button, 255);
        lv_style_set_pad_hor(&style_button, 14);
        lv_style_set_pad_top(&style_button, 5);
        lv_style_set_height(&style_button_360, 40);
        lv_style_set_pad_hor(&style_button_360, 10);
        lv_style_set_pad_top(&style_button_360, 3);
        lv_style_set_height(&style_button_240, 30);
        lv_style_set_pad_hor(&style_button_240, 8);
        lv_style_set_pad_top(&style_button_240, 2);
        lv_style_set_bg_color(&style_pressed, lv_color_hex(0xbebebe));
        lv_style_set_align(&style_icon, LV_ALIGN_CENTER);
        lv_style_set_image_recolor(&style_icon, lv_color_hex(0xffffff));
        lv_style_set_image_recolor_opa(&style_icon, 255);
        lv_style_set_bg_color(&style_checked, lv_color_hex(0x0534ff));

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "hs_button_text_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_EVENT_BUBBLE, true);

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_button, 0);
        lv_obj_add_style(lv_obj_0, &style_pressed, LV_STATE_PRESSED);
        lv_obj_add_style(lv_obj_0, &style_checked, LV_STATE_CHECKED);
        lv_obj_bind_style(lv_obj_0, &style_button_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_button_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_small_0 = hs_text_small_create(lv_obj_0);
        lv_label_set_text(hs_text_small_0, text);
        lv_obj_set_align(hs_text_small_0, LV_ALIGN_CENTER);
        lv_label_set_translation_tag(hs_text_small_0, tag);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

