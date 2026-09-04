/**
 * @file panel_dot_item_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "panel_dot_item_gen.h"
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

lv_obj_t * panel_dot_item_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;
    static lv_style_t style_checked;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);
        lv_style_init(&style_checked);

        lv_style_set_width(&style_base, 9);
        lv_style_set_height(&style_base, 9);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x484848));
        lv_style_set_radius(&style_base, 10);
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_width(&style_base_360, 7);
        lv_style_set_height(&style_base_360, 7);
        lv_style_set_radius(&style_base_360, 8);
        lv_style_set_width(&style_base_240, 5);
        lv_style_set_height(&style_base_240, 5);
        lv_style_set_radius(&style_base_240, 6);
        lv_style_set_bg_color(&style_checked, lv_color_hex(0xffffff));

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "panel_dot_item_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_add_style(lv_obj_0, &style_checked, LV_STATE_CHECKED);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

