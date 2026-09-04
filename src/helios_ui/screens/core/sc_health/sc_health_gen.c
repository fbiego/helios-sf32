/**
 * @file sc_health_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "sc_health_gen.h"
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

lv_obj_t * sc_health_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "sc_health_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * hs_column_0 = hs_column_create(lv_obj_0);
        lv_obj_set_align(hs_column_0, LV_ALIGN_CENTER);
        lv_obj_set_style_flex_track_place(hs_column_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_cross_place(hs_column_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_row(hs_column_0, 30, 0);
        lv_obj_t * wd_image_0 = wd_image_create(hs_column_0);
        wd_image_set_src(wd_image_0, img_finger_sensor);
        lv_obj_set_style_image_recolor(wd_image_0, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_image_recolor_opa(wd_image_0, 255, 0);
        lv_obj_bind_flag_if_eq(wd_image_0, &sb_health_finger_detect, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_column_0);
        lv_label_set_text(hs_text_small_0, "Place finger on the sensor");
        lv_obj_bind_flag_if_eq(hs_text_small_0, &sb_health_finger_detect, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_text_icon_normal_0 = hs_text_icon_normal_create(hs_column_0, img_heart_beat, "Label", "", &sb_health_bpm, "  %d BPM");
        lv_obj_set_width(hs_text_icon_normal_0, lv_pct(75));
        lv_obj_bind_flag_if_eq(hs_text_icon_normal_0, &sb_health_finger_detect, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * hs_text_icon_normal_1 = hs_text_icon_normal_create(hs_column_0, img_blood_oxygen, "Label", "", &sb_health_oxygen, "SP02: %d%%");
        lv_obj_set_width(hs_text_icon_normal_1, lv_pct(75));
        lv_obj_bind_flag_if_eq(hs_text_icon_normal_1, &sb_health_finger_detect, LV_OBJ_FLAG_HIDDEN, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

