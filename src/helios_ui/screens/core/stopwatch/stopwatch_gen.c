/**
 * @file stopwatch_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "stopwatch_gen.h"
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

lv_obj_t * stopwatch_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_list;
    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;
    static lv_style_t style_pad_rect;
    static lv_style_t style_pad_rect_360;
    static lv_style_t style_pad_rect_240;
    static lv_style_t style_pad_lap;
    static lv_style_t style_pad_lap_360;
    static lv_style_t style_pad_lap_240;
    static lv_style_t style_btn_reset;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_list);
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);
        lv_style_init(&style_pad_rect);
        lv_style_init(&style_pad_rect_360);
        lv_style_init(&style_pad_rect_240);
        lv_style_init(&style_pad_lap);
        lv_style_init(&style_pad_lap_360);
        lv_style_init(&style_pad_lap_240);
        lv_style_init(&style_btn_reset);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, lv_pct(100));
        lv_style_set_bg_color(&style_list, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_list, 255);
        lv_style_set_pad_ver(&style_cont, 20);
        lv_style_set_pad_row(&style_cont, 10);
        lv_style_set_pad_hor(&style_cont, 24);
        lv_style_set_pad_ver(&style_cont_360, 15);
        lv_style_set_pad_row(&style_cont_360, 8);
        lv_style_set_pad_hor(&style_cont_360, 18);
        lv_style_set_pad_ver(&style_cont_240, 10);
        lv_style_set_pad_row(&style_cont_240, 4);
        lv_style_set_pad_hor(&style_cont_240, 12);
        lv_style_set_pad_hor(&style_pad_rect, 20);
        lv_style_set_pad_hor(&style_pad_rect_360, 16);
        lv_style_set_pad_hor(&style_pad_rect_240, 12);
        lv_style_set_pad_hor(&style_pad_lap, 40);
        lv_style_set_pad_hor(&style_pad_lap_360, 30);
        lv_style_set_pad_hor(&style_pad_lap_240, 20);
        lv_style_set_bg_color(&style_btn_reset, lv_color_hex(0xa30084));

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "stopwatch_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_t * hs_container_0 = hs_container_create(lv_obj_0);
        lv_obj_set_width(hs_container_0, lv_pct(100));
        lv_obj_set_height(hs_container_0, lv_pct(100));
        lv_obj_set_style_layout(hs_container_0, LV_LAYOUT_FLEX, 0);
        lv_obj_set_flex_flow(hs_container_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_add_style(hs_container_0, &style_cont, 0);
        lv_obj_bind_style(hs_container_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_container_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_bind_state_if_eq(hs_container_0, &sb_screen_type, LV_STATE_USER_2, 1);
        lv_obj_bind_style(hs_container_0, &style_pad_rect, LV_STATE_USER_2, &sb_screen_size, 0);
        lv_obj_bind_style(hs_container_0, &style_pad_rect_360, LV_STATE_USER_2, &sb_screen_size, 1);
        lv_obj_bind_style(hs_container_0, &style_pad_rect_240, LV_STATE_USER_2, &sb_screen_size, 2);
        lv_obj_t * hs_column_0 = hs_column_create(hs_container_0);
        lv_obj_set_style_flex_cross_place(hs_column_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_track_place(hs_column_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_bg_color(hs_column_0, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(hs_column_0, 200, 0);
        lv_obj_set_style_pad_top(hs_column_0, 14, 0);
        lv_obj_set_style_pad_bottom(hs_column_0, 10, 0);
        lv_obj_set_flex_grow(hs_column_0, 1);
        lv_obj_set_flag(hs_column_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_style(hs_column_0, &style_cont, 0);
        lv_obj_bind_style(hs_column_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_column_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_title_pill_0 = hs_title_pill_create(hs_column_0, "stopwatch");
        lv_obj_set_width(hs_title_pill_0, LV_SIZE_CONTENT);

        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_column_0);
        lv_label_bind_text(hs_text_normal_0, &sb_stopwatch_time, NULL);

        hs_line_create(hs_column_0);

        lv_obj_t * lap_container = hs_column_create(hs_column_0);
        lv_obj_set_name(lap_container, "lap_container");
        lv_obj_set_flex_grow(lap_container, 1);
        lv_obj_set_style_pad_row(lap_container, 5, 0);
        hs_lap_item_create(lap_container, "Lap 1", "00:05:34");

        hs_lap_item_create(lap_container, "Lap 1", "00:05:34");

        hs_lap_item_create(lap_container, "Lap 1", "00:05:34");

        hs_lap_item_create(lap_container, "Lap 1", "00:05:34");

        hs_lap_item_create(lap_container, "Lap 1", "00:05:34");

        hs_lap_item_create(lap_container, "Lap 1", "00:05:34");

        hs_lap_item_create(lap_container, "Lap 1", "00:05:34");

        hs_lap_item_create(lap_container, "Lap 1", "00:05:34");

        hs_line_create(hs_column_0);

        lv_obj_t * hs_column_1 = hs_column_create(hs_container_0);
        lv_obj_set_style_flex_cross_place(hs_column_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_align(hs_column_1, LV_ALIGN_BOTTOM_MID);
        lv_obj_set_style_bg_color(hs_column_1, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(hs_column_1, 200, 0);
        lv_obj_set_style_pad_row(hs_column_1, 5, 0);
        lv_obj_t * hs_row_0 = hs_row_create(hs_column_1);
        lv_obj_set_style_flex_main_place(hs_row_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(hs_row_0, 10, 0);
        lv_obj_t * hs_container_1 = hs_container_create(hs_row_0);
        lv_obj_set_flex_grow(hs_container_1, 1);
        lv_obj_bind_flag_if_eq(hs_container_1, &sb_screen_type, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_button_icon_0 = hs_button_icon_create(hs_row_0, icon_arrow_up, 0, &sb_stopwatch_button_reset_icon);
        lv_obj_set_flag(hs_button_icon_0, LV_OBJ_FLAG_CHECKABLE, true);
        lv_obj_bind_checked(hs_button_icon_0, &sb_stopwatch_button_reset_state);
        lv_obj_set_flex_grow(hs_button_icon_0, 2);
        lv_obj_set_style_bg_color(hs_button_icon_0, lv_color_hex(0x9a3a00), 0);
        lv_obj_add_style(hs_button_icon_0, &style_btn_reset, LV_STATE_CHECKED);
        lv_obj_bind_state_if_eq(hs_button_icon_0, &sb_stopwatch_state, LV_STATE_DISABLED, 0);

        lv_obj_t * hs_button_icon_1 = hs_button_icon_create(hs_row_0, icon_arrow_up, 0, &sb_stopwatch_button_start_icon);
        lv_obj_set_flag(hs_button_icon_1, LV_OBJ_FLAG_CHECKABLE, true);
        lv_obj_bind_checked(hs_button_icon_1, &sb_stopwatch_button_start_state);
        lv_obj_set_flex_grow(hs_button_icon_1, 2);
        lv_obj_set_style_bg_color(hs_button_icon_1, lv_color_hex(0x088c00), 0);

        lv_obj_t * hs_container_2 = hs_container_create(hs_row_0);
        lv_obj_set_flex_grow(hs_container_2, 1);
        lv_obj_bind_flag_if_eq(hs_container_2, &sb_screen_type, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_column_1);
        lv_label_bind_text(hs_text_small_0, &sb_time_string, NULL);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

