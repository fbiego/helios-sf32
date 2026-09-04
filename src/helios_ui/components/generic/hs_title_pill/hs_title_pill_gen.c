/**
 * @file hs_title_pill_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_title_pill_gen.h"
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

lv_obj_t * hs_title_pill_create(lv_obj_t * parent, const char * tag)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_pad_360;
    static lv_style_t style_pad_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_pad_360);
        lv_style_init(&style_pad_240);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, LV_SIZE_CONTENT);
        lv_style_set_radius(&style_base, 100);
        lv_style_set_pad_all(&style_base, 10);
        lv_style_set_pad_bottom(&style_base, 3);
        lv_style_set_pad_hor(&style_base, 25);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x171717));
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_pad_all(&style_pad_360, 8);
        lv_style_set_pad_bottom(&style_pad_360, 2);
        lv_style_set_pad_hor(&style_pad_360, 20);
        lv_style_set_pad_all(&style_pad_240, 6);
        lv_style_set_pad_bottom(&style_pad_240, 1);
        lv_style_set_pad_hor(&style_pad_240, 14);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "hs_title_pill_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_pad_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_label_set_translation_tag(hs_text_normal_0, tag);
        lv_label_set_long_mode(hs_text_normal_0, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_align(hs_text_normal_0, LV_ALIGN_CENTER);

        lv_obj_add_event_cb(lv_obj_0, on_hs_title_pill_cb, LV_EVENT_ALL, NULL);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

