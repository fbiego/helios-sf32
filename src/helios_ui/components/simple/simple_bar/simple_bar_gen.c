/**
 * @file simple_bar_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "simple_bar_gen.h"
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

lv_obj_t * simple_bar_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_indicator;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_indicator);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, 6);
        lv_style_set_bg_color(&style_base, lv_color_hex(0xffffff));
        lv_style_set_blend_mode(&style_base, LV_BLEND_MODE_DIFFERENCE);
        lv_style_set_bg_color(&style_indicator, lv_color_hex(0xffffff));
        lv_style_set_height(&style_base_360, 4);
        lv_style_set_height(&style_base_240, 2);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_bar_0 = lv_bar_create(parent);
        lv_obj_set_name_static(lv_bar_0, "simple_bar_#");

        lv_obj_add_style(lv_bar_0, &style_base, 0);
        lv_obj_add_style(lv_bar_0, &style_indicator, LV_PART_INDICATOR);
        lv_obj_bind_style(lv_bar_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_bar_0, &style_base_240, 0, &sb_screen_size, 2);

        the_root = lv_bar_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

