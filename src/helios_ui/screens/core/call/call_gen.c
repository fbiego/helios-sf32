/**
 * @file call_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "call_gen.h"
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

lv_obj_t * call_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, lv_pct(100));
        lv_style_set_pad_ver(&style_cont, 80);
        lv_style_set_pad_row(&style_cont, 40);
        lv_style_set_pad_hor(&style_cont, 10);
        lv_style_set_pad_ver(&style_cont_360, 60);
        lv_style_set_pad_row(&style_cont_360, 30);
        lv_style_set_pad_hor(&style_cont_360, 18);
        lv_style_set_pad_ver(&style_cont_240, 40);
        lv_style_set_pad_row(&style_cont_240, 20);
        lv_style_set_pad_hor(&style_cont_240, 12);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "call_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_t * hs_column_0 = hs_column_create(lv_obj_0);
        lv_obj_set_height(hs_column_0, lv_pct(100));
        lv_obj_set_style_flex_cross_place(hs_column_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_track_place(hs_column_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_add_style(hs_column_0, &style_cont, 0);
        lv_obj_bind_style(hs_column_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_column_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_t * wd_image_0 = wd_image_create(hs_column_0);
        lv_obj_set_flag(wd_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_image_set_src(wd_image_0, img_call);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 190);
        wd_image_set_scale_2(wd_image_0, 120);
        wd_image_set_size_1(wd_image_0, 55);
        wd_image_set_size_2(wd_image_0, 30);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_column_0);
        lv_label_set_translation_tag(hs_text_small_0, "incoming_call");

        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_column_0);
        lv_obj_set_width(hs_text_normal_0, lv_pct(80));
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_bind_text(hs_text_normal_0, &sb_caller_name, NULL);

        lv_obj_t * hs_text_small_1 = hs_text_small_create(lv_obj_0);
        lv_obj_set_align(hs_text_small_1, LV_ALIGN_BOTTOM_MID);
        lv_label_bind_text(hs_text_small_1, &sb_time_string, NULL);
        lv_obj_set_style_text_color(hs_text_small_1, lv_color_hex(0x6d6d6d), 0);
        lv_obj_set_style_pad_bottom(hs_text_small_1, 6, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

