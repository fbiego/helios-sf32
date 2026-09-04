/**
 * @file weather_widget_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "weather_widget_gen.h"
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

lv_obj_t * weather_widget_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_pad_360;
    static lv_style_t style_pad_240;
    static lv_style_t style_cont;
    static lv_style_t style_pad_cont;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_pad_360);
        lv_style_init(&style_pad_240);
        lv_style_init(&style_cont);
        lv_style_init(&style_pad_cont);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, lv_pct(100));
        lv_style_set_pad_row(&style_base, 10);
        lv_style_set_pad_ver(&style_base, 60);
        lv_style_set_text_color(&style_base, lv_color_hex3(0xfff));
        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_base, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_main_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_ver(&style_pad_360, 40);
        lv_style_set_pad_hor(&style_pad_360, 25);
        lv_style_set_pad_row(&style_pad_360, 6);
        lv_style_set_pad_ver(&style_pad_240, 20);
        lv_style_set_pad_hor(&style_pad_240, 20);
        lv_style_set_pad_row(&style_pad_240, 4);
        lv_style_set_width(&style_cont, LV_SIZE_CONTENT);
        lv_style_set_height(&style_cont, LV_SIZE_CONTENT);
        lv_style_set_layout(&style_cont, LV_LAYOUT_FLEX);
        lv_style_set_pad_column(&style_pad_cont, 10);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "weather_widget_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_pad_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_label_bind_text(hs_text_normal_0, &sb_weather_location, NULL);
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(hs_text_normal_0, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_width(hs_text_normal_0, lv_pct(80));

        lv_obj_t * wd_image_0 = wd_image_create(lv_obj_0);
        lv_obj_set_flag(wd_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        wd_image_set_src(wd_image_0, icon_weather);
        wd_image_bind_src(wd_image_0, &sb_weather_icon);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 190);
        wd_image_set_scale_2(wd_image_0, 120);
        wd_image_set_size_1(wd_image_0, 50);
        wd_image_set_size_2(wd_image_0, 30);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);

        lv_obj_t * hs_tag_small_0 = hs_tag_small_create(lv_obj_0);
        wd_label_bind_tag(hs_tag_small_0, &sb_weather_condition);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_remove_style_all(lv_obj_1);
        lv_obj_add_style(lv_obj_1, &style_cont, 0);
        lv_obj_t * hs_text_large_0 = hs_text_large_create(lv_obj_1);
        lv_label_bind_text(hs_text_large_0, &sb_weather_temp, NULL);

        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(lv_obj_1);
        lv_label_bind_text(hs_text_normal_1, &sb_weather_temp_unit, "°%s");
        lv_obj_set_style_text_align(hs_text_normal_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_top(hs_text_normal_1, 15, 0);

        lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
        lv_obj_remove_style_all(lv_obj_2);
        lv_obj_add_style(lv_obj_2, &style_cont, 0);
        lv_obj_add_style(lv_obj_2, &style_pad_cont, 0);
        hs_text_icon_normal_create(lv_obj_2, icon_weather_temp_up_icon, "Label", "", &sb_weather_temp_high, "%d°");

        hs_text_icon_normal_create(lv_obj_2, icon_weather_temp_down_icon, "Label", "", &sb_weather_temp_low, "%d°");

        lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_0);
        lv_obj_remove_style_all(lv_obj_3);
        lv_obj_add_style(lv_obj_3, &style_cont, 0);
        lv_obj_add_style(lv_obj_3, &style_pad_cont, 0);
        hs_text_icon_normal_create(lv_obj_3, icon_weather_humidity_icon, "Label", "", &sb_weather_humidity, "%d");

        hs_text_icon_normal_create(lv_obj_3, icon_weather_uv_icon, "Label", "", &sb_weather_uv, "%d");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

