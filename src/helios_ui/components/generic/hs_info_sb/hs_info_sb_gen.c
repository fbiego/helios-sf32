/**
 * @file hs_info_sb_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_info_sb_gen.h"
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

lv_obj_t * hs_info_sb_create(lv_obj_t * parent, const char * label, const char * label_tag, lv_subject_t * info, const char * fmt)
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
        lv_style_set_radius(&style_base, 10);
        lv_style_set_pad_ver(&style_base, 4);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x171717));
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_pad_ver(&style_pad_360, 3);
        lv_style_set_radius(&style_pad_360, 6);
        lv_style_set_pad_ver(&style_pad_240, 2);
        lv_style_set_radius(&style_pad_240, 3);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "hs_info_sb_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_pad_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_label_set_text(hs_text_normal_0, label);
        lv_label_set_translation_tag(hs_text_normal_0, label_tag);
        lv_obj_set_width(hs_text_normal_0, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_right(hs_text_normal_0, 5, 0);
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_LEFT, 0);

        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(lv_obj_0);
        lv_label_bind_text(hs_text_normal_1, info, fmt);
        lv_obj_set_align(hs_text_normal_1, LV_ALIGN_TOP_RIGHT);
        lv_obj_set_width(hs_text_normal_1, LV_SIZE_CONTENT);
        lv_obj_set_style_text_align(hs_text_normal_1, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_style_text_color(hs_text_normal_1, lv_color_hex3(0xaaa), 0);

        lv_obj_add_event_cb(lv_obj_0, on_hs_info_cb, LV_EVENT_ALL, NULL);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

