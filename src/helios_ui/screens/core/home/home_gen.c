/**
 * @file home_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "home_gen.h"
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

lv_obj_t * home_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "home_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * watchface_cont = hs_container_create(lv_obj_0);
        lv_obj_set_name(watchface_cont, "watchface_cont");
        lv_obj_set_width(watchface_cont, lv_pct(100));
        lv_obj_set_height(watchface_cont, lv_pct(100));
        lv_obj_set_flag(watchface_cont, LV_OBJ_FLAG_CLICKABLE, false);
        watchface_default_create(watchface_cont);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

