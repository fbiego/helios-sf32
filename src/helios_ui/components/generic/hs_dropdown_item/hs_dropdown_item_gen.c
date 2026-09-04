/**
 * @file hs_dropdown_item_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_dropdown_item_gen.h"
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

lv_obj_t * hs_dropdown_item_create(lv_obj_t * parent, const char * text, const char * tag)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_dropdown_360;
    static lv_style_t style_dropdown_240;
    static lv_style_t style_pressed;
    static lv_style_t style_checked;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_dropdown_360);
        lv_style_init(&style_dropdown_240);
        lv_style_init(&style_pressed);
        lv_style_init(&style_checked);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_bg_color(&style_base, lv_color_hex(0x3a3a3a));
        lv_style_set_bg_opa(&style_base, 0);
        lv_style_set_height(&style_base, 46);
        lv_style_set_text_color(&style_base, lv_color_hex(0xffffff));
        lv_style_set_border_width(&style_base, 0);
        lv_style_set_radius(&style_base, 6);
        lv_style_set_pad_all(&style_base, 0);
        lv_style_set_pad_top(&style_base, 5);
        lv_style_set_pad_hor(&style_base, 10);
        lv_style_set_height(&style_dropdown_360, 36);
        lv_style_set_radius(&style_dropdown_360, 4);
        lv_style_set_pad_top(&style_dropdown_360, 3);
        lv_style_set_pad_hor(&style_dropdown_360, 8);
        lv_style_set_height(&style_dropdown_240, 24);
        lv_style_set_radius(&style_dropdown_240, 2);
        lv_style_set_pad_top(&style_dropdown_240, 3);
        lv_style_set_pad_hor(&style_dropdown_240, 5);
        lv_style_set_bg_color(&style_pressed, lv_color_hex(0x989898));
        lv_style_set_bg_opa(&style_pressed, 255);
        lv_style_set_text_color(&style_pressed, lv_color_hex3(0x000));
        lv_style_set_bg_color(&style_checked, COLOR_PRIMARY);
        lv_style_set_bg_opa(&style_checked, 255);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "hs_dropdown_item_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_dropdown_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_dropdown_240, 0, &sb_screen_size, 2);
        lv_obj_add_style(lv_obj_0, &style_checked, LV_STATE_CHECKED);
        lv_obj_add_style(lv_obj_0, &style_pressed, LV_STATE_PRESSED);
        lv_obj_t * hs_text_small_0 = hs_text_small_create(lv_obj_0);
        lv_label_set_text(hs_text_small_0, text);
        lv_label_set_translation_tag(hs_text_small_0, tag);
        lv_obj_set_align(hs_text_small_0, LV_ALIGN_CENTER);
        lv_obj_set_width(hs_text_small_0, lv_pct(100));

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

