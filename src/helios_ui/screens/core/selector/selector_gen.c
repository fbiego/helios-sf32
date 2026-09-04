/**
 * @file selector_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "selector_gen.h"
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

lv_obj_t * selector_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_cont;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_cont);

        lv_style_set_width(&style_cont, lv_pct(100));
        lv_style_set_height(&style_cont, lv_pct(100));
        lv_style_set_layout(&style_cont, LV_LAYOUT_FLEX);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "selector_#");
        lv_obj_set_scrollbar_mode(lv_obj_0, LV_SCROLLBAR_MODE_OFF);

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * selector_cont = wd_panel_create(lv_obj_0);
        lv_obj_set_name(selector_cont, "selector_cont");
        lv_obj_t * wd_panel_container_0 = wd_panel_get_container(selector_cont);
        lv_obj_set_scroll_snap_x(wd_panel_container_0, LV_SCROLL_SNAP_CENTER);
        lv_obj_set_style_flex_track_place(wd_panel_container_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_add_style(wd_panel_container_0, &style_cont, 0);
        face_preview_create(wd_panel_container_0, img_preview_466, "Default");

        face_preview_create(wd_panel_container_0, img_preview_466, "Default");

        face_preview_create(wd_panel_container_0, img_preview_466, "Default");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

