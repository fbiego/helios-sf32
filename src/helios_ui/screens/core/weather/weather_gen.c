/**
 * @file weather_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "weather_gen.h"
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

lv_obj_t * weather_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_list;
    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;
    static lv_style_t style_pad_rect;
    static lv_style_t style_pad_rect_360;
    static lv_style_t style_pad_rect_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_list);
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);
        lv_style_init(&style_pad_rect);
        lv_style_init(&style_pad_rect_360);
        lv_style_init(&style_pad_rect_240);

        lv_style_set_bg_color(&style_list, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_list, 255);
        lv_style_set_pad_top(&style_cont, 0);
        lv_style_set_pad_bottom(&style_cont, 150);
        lv_style_set_pad_hor(&style_cont, 33);
        lv_style_set_pad_hor(&style_cont_360, 30);
        lv_style_set_pad_top(&style_cont_360, 0);
        lv_style_set_pad_bottom(&style_cont_360, 100);
        lv_style_set_pad_hor(&style_cont_240, 20);
        lv_style_set_pad_top(&style_cont_240, 10);
        lv_style_set_pad_bottom(&style_cont_240, 80);
        lv_style_set_pad_hor(&style_pad_rect, 18);
        lv_style_set_pad_hor(&style_pad_rect_360, 14);
        lv_style_set_pad_hor(&style_pad_rect_240, 10);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "weather_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * weather_list = wd_list_create(lv_obj_0);
        lv_obj_set_name(weather_list, "weather_list");
        wd_list_bind_screen(weather_list, &sb_screen_size);
        lv_obj_add_style(weather_list, &style_list, 0);
        lv_obj_t * wd_list_container_0 = wd_list_get_container(weather_list);
        lv_obj_bind_style(wd_list_container_0, &style_cont, 0, &sb_screen_size, 0);
        lv_obj_bind_style(wd_list_container_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_bind_state_if_eq(wd_list_container_0, &sb_screen_type, LV_STATE_USER_2, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect, LV_STATE_USER_2, &sb_screen_size, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_360, LV_STATE_USER_2, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_240, LV_STATE_USER_2, &sb_screen_size, 2);
        lv_obj_t * weather_widget_0 = weather_widget_create(wd_list_container_0);
        lv_obj_set_height(weather_widget_0, LV_SIZE_CONTENT);

        lv_obj_t * hs_card_0 = hs_card_create(wd_list_container_0);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_card_0);
        lv_obj_set_width(hs_text_normal_0, lv_pct(100));
        lv_label_set_text(hs_text_normal_0, "Hourly Forecast");
        lv_label_set_translation_tag(hs_text_normal_0, "hourly_forecast");
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(hs_text_normal_0, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);

        hs_line_create(hs_card_0);

        lv_obj_t * hourly_cont = hs_row_create(hs_card_0);
        lv_obj_set_name(hourly_cont, "hourly_cont");
        lv_obj_set_style_pad_column(hourly_cont, 10, 0);
        hourly_forecast_create(hourly_cont);

        hourly_forecast_create(hourly_cont);

        hourly_forecast_create(hourly_cont);

        hourly_forecast_create(hourly_cont);

        hourly_forecast_create(hourly_cont);

        hourly_forecast_create(hourly_cont);

        hourly_forecast_create(hourly_cont);

        lv_obj_t * hs_card_1 = hs_card_create(wd_list_container_0);
        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(hs_card_1);
        lv_obj_set_width(hs_text_normal_1, lv_pct(100));
        lv_label_set_text(hs_text_normal_1, "Daily Forecast");
        lv_label_set_translation_tag(hs_text_normal_1, "daily_forecast");
        lv_obj_set_style_text_align(hs_text_normal_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(hs_text_normal_1, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);

        lv_obj_t * daily_cont = hs_column_create(hs_card_1);
        lv_obj_set_name(daily_cont, "daily_cont");
        lv_obj_set_style_pad_row(daily_cont, 2, 0);
        daily_forecast_create(daily_cont);

        daily_forecast_create(daily_cont);

        daily_forecast_create(daily_cont);

        daily_forecast_create(daily_cont);

        daily_forecast_create(daily_cont);

        daily_forecast_create(daily_cont);

        daily_forecast_create(daily_cont);

        lv_obj_t * hs_column_0 = hs_column_create(wd_list_container_0);
        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_column_0);
        lv_obj_set_width(hs_text_small_0, lv_pct(100));
        lv_label_set_translation_tag(hs_text_small_0, "updated_at");
        lv_obj_set_style_text_align(hs_text_small_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * hs_text_small_1 = hs_text_small_create(hs_column_0);
        lv_obj_set_width(hs_text_small_1, lv_pct(100));
        lv_label_bind_text(hs_text_small_1, &sb_weather_update_time, NULL);
        lv_obj_set_style_text_align(hs_text_small_1, LV_TEXT_ALIGN_CENTER, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

