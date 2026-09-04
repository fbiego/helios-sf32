/**
 * @file hs_lap_item_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_lap_item_gen.h"
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

lv_obj_t * hs_lap_item_create(lv_obj_t * parent, const char * title, const char * time)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);

        lv_style_set_flex_main_place(&style_base, LV_FLEX_ALIGN_SPACE_BETWEEN);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * hs_row_0 = hs_row_create(parent);
        lv_obj_set_name_static(hs_row_0, "hs_lap_item_#");

        lv_obj_add_style(hs_row_0, &style_base, 0);
        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_row_0);
        lv_label_set_text(hs_text_small_0, title);

        lv_obj_t * hs_text_small_1 = hs_text_small_create(hs_row_0);
        lv_label_set_text(hs_text_small_1, time);

        the_root = hs_row_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

