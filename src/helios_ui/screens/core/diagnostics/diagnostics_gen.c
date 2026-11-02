/**
 * @file diagnostics_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "diagnostics_gen.h"
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

lv_obj_t * diagnostics_create(void)
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

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "diagnostics_#");

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
        lv_obj_add_style(hs_column_0, &style_cont, 0);
        lv_obj_bind_style(hs_column_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_column_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_title_pill_0 = hs_title_pill_create(hs_column_0, "Diagnostics");
        lv_obj_set_width(hs_title_pill_0, LV_SIZE_CONTENT);

        lv_obj_t * hs_card_0 = hs_card_create(hs_column_0);
        hs_title_create(hs_card_0, "BMI270", "", &sb_battery_percent);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_card_0);
        lv_label_bind_text(hs_text_small_0, &sb_diagnostic_bmi270, NULL);

        lv_obj_t * hs_card_1 = hs_card_create(hs_column_0);
        hs_title_create(hs_card_1, "Battery", "", &sb_battery_percent);

        lv_obj_t * hs_text_small_1 = hs_text_small_create(hs_card_1);
        lv_label_bind_text(hs_text_small_1, &sb_diagnostic_battery, NULL);

        lv_obj_t * hs_card_2 = hs_card_create(hs_column_0);
        hs_title_create(hs_card_2, "Haptics", "", &sb_battery_percent);

        lv_obj_t * hs_row_0 = hs_row_create(hs_card_2);
        lv_obj_set_style_pad_column(hs_row_0, 10, 0);
        lv_obj_t * hs_button_text_0 = hs_button_text_create(hs_row_0, "Vibration", "", 0);
        lv_obj_add_event_cb(hs_button_text_0, on_diagnostic_click, LV_EVENT_CLICKED, "vibration");

        lv_obj_t * hs_button_text_1 = hs_button_text_create(hs_row_0, "Buzzer", "", 0);
        lv_obj_add_event_cb(hs_button_text_1, on_diagnostic_click, LV_EVENT_CLICKED, "buzzer");

        hs_space_create(hs_column_0);

        hs_space_create(hs_column_0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

