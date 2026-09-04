/**
 * @file timer_list_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "timer_list_gen.h"
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

lv_obj_t * timer_list_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_list;
    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_list);
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, lv_pct(100));
        lv_style_set_bg_color(&style_list, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_list, 255);
        lv_style_set_pad_ver(&style_cont, 30);
        lv_style_set_pad_row(&style_cont, 18);
        lv_style_set_pad_ver(&style_cont_360, 20);
        lv_style_set_pad_row(&style_cont_360, 8);
        lv_style_set_pad_ver(&style_cont_240, 12);
        lv_style_set_pad_row(&style_cont_240, 4);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "timer_list_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_t * hs_column_0 = hs_column_create(lv_obj_0);
        lv_obj_set_flex_flow(hs_column_0, LV_FLEX_FLOW_ROW_WRAP);
        lv_obj_set_style_pad_top(hs_column_0, 100, 0);
        lv_obj_t * hs_button_0 = hs_button_create(hs_column_0, "1 min", "", COLOR_PRIMARY);
        lv_obj_set_width(hs_button_0, lv_pct(50));
        lv_obj_set_style_radius(hs_button_0, 200, 0);
        lv_obj_set_style_margin_all(hs_button_0, 10, 0);

        lv_obj_t * hs_button_1 = hs_button_create(hs_column_0, "2 mins", "", COLOR_PRIMARY);
        lv_obj_set_width(hs_button_1, lv_pct(50));
        lv_obj_set_style_radius(hs_button_1, 200, 0);
        lv_obj_set_style_margin_all(hs_button_1, 10, 0);

        lv_obj_t * hs_button_2 = hs_button_create(hs_column_0, "5 mins", "", COLOR_PRIMARY);
        lv_obj_set_width(hs_button_2, lv_pct(50));
        lv_obj_set_style_radius(hs_button_2, 200, 0);
        lv_obj_set_style_margin_all(hs_button_2, 10, 0);

        lv_obj_t * hs_button_3 = hs_button_create(hs_column_0, "10 mins", "", COLOR_PRIMARY);
        lv_obj_set_width(hs_button_3, lv_pct(50));
        lv_obj_set_style_radius(hs_button_3, 200, 0);
        lv_obj_set_style_margin_all(hs_button_3, 10, 0);

        lv_obj_t * hs_button_4 = hs_button_create(hs_column_0, "15 mins", "", COLOR_PRIMARY);
        lv_obj_set_width(hs_button_4, lv_pct(50));
        lv_obj_set_style_radius(hs_button_4, 200, 0);
        lv_obj_set_style_margin_all(hs_button_4, 10, 0);

        lv_obj_t * hs_button_5 = hs_button_create(hs_column_0, "20 mins", "", COLOR_PRIMARY);
        lv_obj_set_width(hs_button_5, lv_pct(50));
        lv_obj_set_style_radius(hs_button_5, 200, 0);
        lv_obj_set_style_margin_all(hs_button_5, 10, 0);

        lv_obj_t * hs_button_6 = hs_button_create(hs_column_0, "30 mins", "", COLOR_PRIMARY);
        lv_obj_set_width(hs_button_6, lv_pct(50));
        lv_obj_set_style_radius(hs_button_6, 200, 0);
        lv_obj_set_style_margin_all(hs_button_6, 10, 0);

        lv_obj_t * hs_button_7 = hs_button_create(hs_column_0, "1 hr", "", COLOR_PRIMARY);
        lv_obj_set_width(hs_button_7, lv_pct(50));
        lv_obj_set_style_radius(hs_button_7, 200, 0);
        lv_obj_set_style_margin_all(hs_button_7, 10, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

