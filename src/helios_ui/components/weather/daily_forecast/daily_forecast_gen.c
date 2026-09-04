/**
 * @file daily_forecast_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "daily_forecast_gen.h"
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

lv_obj_t * daily_forecast_create(lv_obj_t * parent)
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
        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_main_place(&style_base, LV_FLEX_ALIGN_SPACE_AROUND);
        lv_style_set_text_color(&style_base, lv_color_hex3(0xfff));
        lv_style_set_border_color(&style_base, lv_color_hex(0x626262));
        lv_style_set_border_opa(&style_base, 255);
        lv_style_set_border_width(&style_base, 1);
        lv_style_set_border_side(&style_base, LV_BORDER_SIDE_TOP);
        lv_style_set_pad_top(&style_base, 10);
        lv_style_set_pad_top(&style_pad_360, 8);
        lv_style_set_pad_top(&style_pad_240, 5);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "daily_forecast_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_pad_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_240, 0, &sb_screen_size, 2);
        lv_obj_bind_style(lv_obj_0, &style_text_normal_466, 0, &sb_screen_size, 0);
        lv_obj_bind_style(lv_obj_0, &style_text_normal_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_text_normal_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_obj_set_flex_grow(hs_text_normal_0, 2);
        lv_label_set_text(hs_text_normal_0, "Sun");
        lv_label_set_translation_tag(hs_text_normal_0, "sun");
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * wd_image_0 = wd_image_create(lv_obj_0);
        wd_image_set_src(wd_image_0, icon_weather);
        wd_image_set_scale_0(wd_image_0, 120);
        wd_image_set_scale_1(wd_image_0, 110);
        wd_image_set_scale_2(wd_image_0, 70);
        wd_image_set_size_0(wd_image_0, 30);
        wd_image_set_size_1(wd_image_0, 25);
        wd_image_set_size_2(wd_image_0, 15);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);
        lv_obj_set_flex_grow(wd_image_0, 1);

        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(lv_obj_0);
        lv_obj_set_flex_grow(hs_text_normal_1, 2);
        lv_label_set_text(hs_text_normal_1, "19°/12°");
        lv_obj_set_style_text_align(hs_text_normal_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_letter_space(hs_text_normal_1, -1, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

