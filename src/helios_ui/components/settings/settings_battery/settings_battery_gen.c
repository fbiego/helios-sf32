/**
 * @file settings_battery_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "settings_battery_gen.h"
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

lv_obj_t * settings_battery_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_pad_360;
    static lv_style_t style_pad_240;
    static lv_style_t style_pad_rect;
    static lv_style_t style_pad_rect_360;
    static lv_style_t style_pad_rect_240;
    static lv_style_t style_chart;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_pad_360);
        lv_style_init(&style_pad_240);
        lv_style_init(&style_pad_rect);
        lv_style_init(&style_pad_rect_360);
        lv_style_init(&style_pad_rect_240);
        lv_style_init(&style_chart);

        lv_style_set_pad_row(&style_base, 10);
        lv_style_set_pad_ver(&style_base, 120);
        lv_style_set_pad_ver(&style_pad_360, 90);
        lv_style_set_pad_hor(&style_pad_360, 25);
        lv_style_set_pad_ver(&style_pad_240, 60);
        lv_style_set_pad_hor(&style_pad_240, 20);
        lv_style_set_pad_hor(&style_pad_rect, 18);
        lv_style_set_pad_hor(&style_pad_rect_360, 14);
        lv_style_set_pad_hor(&style_pad_rect_240, 10);
        lv_style_set_height(&style_chart, 100);
        lv_style_set_bg_opa(&style_chart, 0);
        lv_style_set_border_width(&style_chart, 0);
        lv_style_set_radius(&style_chart, 0);
        lv_style_set_pad_column(&style_chart, 2);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * wd_list_0 = wd_list_create(parent);
        lv_obj_set_name_static(wd_list_0, "settings_battery_#");
        wd_list_bind_screen(wd_list_0, &sb_screen_size);

        lv_obj_t * wd_list_title_0 = wd_list_get_title(wd_list_0);
        lv_obj_set_height(wd_list_title_0, LV_SIZE_CONTENT);
        lv_obj_t * hs_title_pill_0 = hs_title_pill_create(wd_list_title_0, "battery");
        lv_obj_set_width(hs_title_pill_0, LV_SIZE_CONTENT);
        lv_obj_set_align(hs_title_pill_0, LV_ALIGN_CENTER);
        lv_obj_set_style_max_width(hs_title_pill_0, lv_pct(100), 0);

        lv_obj_t * wd_list_container_0 = wd_list_get_container(wd_list_0);
        lv_obj_add_style(wd_list_container_0, &style_base, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_240, 0, &sb_screen_size, 2);
        lv_obj_bind_state_if_eq(wd_list_container_0, &sb_screen_type, LV_STATE_USER_1, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect, LV_STATE_USER_1, &sb_screen_size, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_360, LV_STATE_USER_1, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_240, LV_STATE_USER_1, &sb_screen_size, 2);
        lv_obj_t * hs_card_0 = hs_card_create(wd_list_container_0);
        hs_bar_create(hs_card_0, "Brightness", "battery", &sb_battery_percent, lv_color_hex(0xffffff));

        hs_info_sb_create(hs_card_0, "Label", "voltage", &sb_battery_voltage, "%.2fV");

        hs_info_sb_create(hs_card_0, "Label", "temperature", &sb_battery_temp, "%.1f°");

        hs_line_create(hs_card_0);

        lv_obj_t * battery_chart = lv_chart_create(hs_card_0);
        lv_obj_set_name(battery_chart, "battery_chart");
        lv_chart_set_type(battery_chart, LV_CHART_TYPE_BAR);
        lv_chart_set_point_count(battery_chart, 24);
        lv_chart_set_hor_div_line_count(battery_chart, 0);
        lv_chart_set_ver_div_line_count(battery_chart, 0);
        lv_obj_add_style(battery_chart, &style_chart, 0);
        lv_chart_set_axis_min_value(battery_chart, LV_CHART_AXIS_PRIMARY_Y, 0);
        lv_chart_set_axis_max_value(battery_chart, LV_CHART_AXIS_PRIMARY_Y, 100);
        lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(battery_chart, lv_color_hex(0xffffff), LV_CHART_AXIS_PRIMARY_Y);
        static const int32_t battery_chart_values_0[] = {60, 43, 34, 24, 38, 54, 54, 20, 45, 0, 25, 56, 80, 34, 65, 45, 56, 50, 35, 15, 34, 65, 25, 56};
        lv_chart_set_series_values(battery_chart, lv_chart_series_0, battery_chart_values_0, 24);

        hs_line_create(hs_card_0);

        hs_info_sb_create(hs_card_0, "Label", "remaining", &sb_battery_usage, "%s");

        the_root = wd_list_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

