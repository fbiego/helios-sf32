/**
 * @file hs_roller_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_roller_gen.h"
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

lv_obj_t * hs_roller_create(lv_obj_t * parent, const char * options)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;
    static lv_style_t style_selected;
    static lv_style_t style_selected_360;
    static lv_style_t style_selected_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);
        lv_style_init(&style_selected);
        lv_style_init(&style_selected_360);
        lv_style_init(&style_selected_240);

        lv_style_set_height(&style_base, 130);
        lv_style_set_border_width(&style_base, 0);
        lv_style_set_bg_opa(&style_base, 0);
        lv_style_set_radius(&style_base, 0);
        lv_style_set_text_font(&style_base, NS_Medium_30);
        lv_style_set_text_color(&style_base, lv_color_hex(0xffffff));
        lv_style_set_height(&style_base_360, 110);
        lv_style_set_text_font(&style_base_360, NS_Medium_20);
        lv_style_set_height(&style_base_240, 100);
        lv_style_set_text_font(&style_base_240, NS_Medium_18);
        lv_style_set_bg_color(&style_selected, lv_color_hex(0x222222));
        lv_style_set_radius(&style_selected, 5);
        lv_style_set_radius(&style_selected_360, 4);
        lv_style_set_radius(&style_selected_240, 3);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_roller_0 = lv_roller_create(parent);
        lv_obj_set_name_static(lv_roller_0, "hs_roller_#");
        lv_roller_set_options(lv_roller_0, options, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_selected(lv_roller_0, 3, false);

        lv_obj_add_style(lv_roller_0, &style_base, 0);
        lv_obj_bind_style(lv_roller_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_roller_0, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_add_style(lv_roller_0, &style_selected, LV_PART_SELECTED);
        lv_obj_bind_style(lv_roller_0, &style_selected_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_roller_0, &style_selected_240, 0, &sb_screen_size, 2);

        the_root = lv_roller_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

