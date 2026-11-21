/**
 * @file hs_arc_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_arc_gen.h"
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

lv_obj_t * hs_arc_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_arc;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_arc);

        lv_style_set_arc_color(&style_arc, lv_color_hex(0x2fc547));
        lv_style_set_arc_width(&style_arc, 40);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_arc_0 = lv_arc_create(parent);
        lv_obj_set_name_static(lv_arc_0, "hs_arc_#");
        lv_arc_set_bg_start_angle(lv_arc_0, 0);
        lv_arc_set_bg_end_angle(lv_arc_0, 360);
        lv_arc_set_rotation(lv_arc_0, 90);

        lv_obj_remove_style(lv_arc_0, NULL, LV_PART_KNOB);
        lv_obj_add_style(lv_arc_0, &style_arc, 0);
        lv_obj_add_style(lv_arc_0, &style_arc, LV_PART_INDICATOR);

        the_root = lv_arc_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

