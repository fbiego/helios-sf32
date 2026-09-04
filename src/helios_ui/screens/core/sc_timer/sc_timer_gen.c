/**
 * @file sc_timer_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "sc_timer_gen.h"
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

lv_obj_t * sc_timer_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "sc_timer_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * hs_title_pill_0 = hs_title_pill_create(lv_obj_0, "timer");
        lv_obj_set_width(hs_title_pill_0, LV_SIZE_CONTENT);
        lv_obj_set_align(hs_title_pill_0, LV_ALIGN_TOP_MID);
        lv_obj_set_y(hs_title_pill_0, 50);

        timer_countdown_create(lv_obj_0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

