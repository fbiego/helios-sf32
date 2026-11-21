/**
 * @file fitness_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "fitness_gen.h"
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

lv_obj_t * fitness_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_pad_360;
    static lv_style_t style_pad_240;
    static lv_style_t style_pad_rect;
    static lv_style_t style_pad_rect_360;
    static lv_style_t style_pad_rect_240;
    static lv_style_t style_chart;
    static lv_style_t style_arc;
    static lv_style_t style_arc_360;
    static lv_style_t style_arc_240;

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
        lv_style_init(&style_arc);
        lv_style_init(&style_arc_360);
        lv_style_init(&style_arc_240);

        lv_style_set_flex_main_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_row(&style_base, 30);
        lv_style_set_pad_ver(&style_base, 60);
        lv_style_set_pad_bottom(&style_base, 120);
        lv_style_set_pad_ver(&style_pad_360, 36);
        lv_style_set_pad_hor(&style_pad_360, 25);
        lv_style_set_pad_bottom(&style_pad_360, 100);
        lv_style_set_pad_ver(&style_pad_240, 30);
        lv_style_set_pad_hor(&style_pad_240, 20);
        lv_style_set_pad_bottom(&style_pad_240, 80);
        lv_style_set_pad_hor(&style_pad_rect, 18);
        lv_style_set_pad_hor(&style_pad_rect_360, 14);
        lv_style_set_pad_hor(&style_pad_rect_240, 10);
        lv_style_set_height(&style_chart, 100);
        lv_style_set_bg_opa(&style_chart, 0);
        lv_style_set_border_width(&style_chart, 0);
        lv_style_set_radius(&style_chart, 0);
        lv_style_set_pad_column(&style_chart, 2);
        lv_style_set_width(&style_arc, 360);
        lv_style_set_height(&style_arc, 360);
        lv_style_set_width(&style_arc_360, 300);
        lv_style_set_height(&style_arc_360, 300);
        lv_style_set_arc_width(&style_arc_360, 30);
        lv_style_set_width(&style_arc_240, 180);
        lv_style_set_height(&style_arc_240, 180);
        lv_style_set_arc_width(&style_arc_240, 20);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "fitness_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * wd_list_0 = wd_list_create(lv_obj_0);
        wd_list_bind_screen(wd_list_0, &sb_screen_size);
        lv_obj_t * wd_list_container_0 = wd_list_get_container(wd_list_0);
        lv_obj_add_style(wd_list_container_0, &style_base, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_240, 0, &sb_screen_size, 2);
        lv_obj_bind_state_if_eq(wd_list_container_0, &sb_screen_type, LV_STATE_USER_1, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect, LV_STATE_USER_1, &sb_screen_size, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_360, LV_STATE_USER_1, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_240, LV_STATE_USER_1, &sb_screen_size, 2);
        lv_obj_t * hs_arc_0 = hs_arc_create(wd_list_container_0);
        lv_obj_set_style_arc_color(hs_arc_0, lv_color_hex(0x18261f), 0);
        lv_obj_set_align(hs_arc_0, LV_ALIGN_CENTER);
        lv_arc_bind_value(hs_arc_0, &sb_activity_progress);
        lv_obj_set_flag(hs_arc_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_layout(hs_arc_0, LV_LAYOUT_FLEX, 0);
        lv_obj_set_flex_flow(hs_arc_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_flex_main_place(hs_arc_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_cross_place(hs_arc_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_track_place(hs_arc_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_add_style(hs_arc_0, &style_arc, 0);
        lv_obj_bind_style(hs_arc_0, &style_arc_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_arc_0, &style_arc_360, LV_PART_INDICATOR, &sb_screen_size, 1);
        lv_obj_bind_style(hs_arc_0, &style_arc_240, 0, &sb_screen_size, 2);
        lv_obj_bind_style(hs_arc_0, &style_arc_240, LV_PART_INDICATOR, &sb_screen_size, 2);
        hs_text_icon_normal_create(hs_arc_0, icon_steps, "Label", "", &sb_activity_steps, "%d");

        hs_text_icon_normal_create(hs_arc_0, icon_calories, "Label", "", &sb_activity_kcal, "%d");

        hs_line_create(wd_list_container_0);

        lv_obj_t * steps_chart = lv_chart_create(wd_list_container_0);
        lv_obj_set_name(steps_chart, "steps_chart");
        lv_chart_set_type(steps_chart, LV_CHART_TYPE_BAR);
        lv_chart_set_point_count(steps_chart, 24);
        lv_chart_set_hor_div_line_count(steps_chart, 0);
        lv_chart_set_ver_div_line_count(steps_chart, 0);
        lv_obj_add_style(steps_chart, &style_chart, 0);
        lv_chart_set_axis_min_value(steps_chart, LV_CHART_AXIS_PRIMARY_Y, 0);
        lv_chart_set_axis_max_value(steps_chart, LV_CHART_AXIS_PRIMARY_Y, 100);
        lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(steps_chart, lv_color_hex(0xffffff), LV_CHART_AXIS_PRIMARY_Y);
        static const int32_t steps_chart_values_0[] = {60, 43, 34, 24, 38, 54, 54, 20, 45, 0, 25, 56, 80, 34, 65, 45, 56, 50, 35, 15, 34, 65, 25, 56};
        lv_chart_set_series_values(steps_chart, lv_chart_series_0, steps_chart_values_0, 24);

        hs_line_create(wd_list_container_0);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(lv_obj_0);
        lv_obj_set_align(hs_text_small_0, LV_ALIGN_BOTTOM_MID);
        lv_label_bind_text(hs_text_small_0, &sb_time_string, NULL);
        lv_obj_set_style_text_color(hs_text_small_0, lv_color_hex(0x6d6d6d), 0);
        lv_obj_set_style_pad_bottom(hs_text_small_0, 6, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

