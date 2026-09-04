/**
 * @file simulator_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "simulator_gen.h"
#include "../../helios_ui.h"

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

lv_obj_t * simulator_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "simulator_#");
        lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x000000), 0);

        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_label_set_text(hs_text_normal_0, "Simulator");
        lv_obj_set_align(hs_text_normal_0, LV_ALIGN_CENTER);

        lv_obj_add_event_cb(lv_obj_0, on_simulator_event_cb, LV_EVENT_ALL, NULL);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

