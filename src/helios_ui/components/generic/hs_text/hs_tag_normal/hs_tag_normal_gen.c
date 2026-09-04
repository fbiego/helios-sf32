/**
 * @file hs_tag_normal_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_tag_normal_gen.h"
#include "../../../../helios_ui.h"

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

lv_obj_t * hs_tag_normal_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);

        lv_style_set_text_color(&style_base, lv_color_hex(0xffffff));

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * wd_label_0 = wd_label_create(parent);
        lv_obj_set_name_static(wd_label_0, "hs_tag_normal_#");

        lv_obj_add_style(wd_label_0, &style_base, 0);
        lv_obj_bind_style(wd_label_0, &style_text_normal_466, 0, &sb_screen_size, 0);
        lv_obj_bind_style(wd_label_0, &style_text_normal_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_label_0, &style_text_normal_240, 0, &sb_screen_size, 2);

        the_root = wd_label_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

