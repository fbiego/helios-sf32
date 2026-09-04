/**
 * @file phone_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "phone_gen.h"
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

lv_obj_t * phone_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;
    static lv_style_t style_flex;
    static lv_style_t style_pad_rect;
    static lv_style_t style_pad_rect_360;
    static lv_style_t style_pad_rect_240;
    static lv_style_t style_pad_none;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);
        lv_style_init(&style_flex);
        lv_style_init(&style_pad_rect);
        lv_style_init(&style_pad_rect_360);
        lv_style_init(&style_pad_rect_240);
        lv_style_init(&style_pad_none);

        lv_style_set_pad_top(&style_cont, 150);
        lv_style_set_pad_bottom(&style_cont, 150);
        lv_style_set_pad_hor(&style_cont, 33);
        lv_style_set_pad_hor(&style_cont_360, 30);
        lv_style_set_pad_top(&style_cont_360, 100);
        lv_style_set_pad_bottom(&style_cont_360, 100);
        lv_style_set_pad_hor(&style_cont_240, 20);
        lv_style_set_pad_row(&style_cont_240, 5);
        lv_style_set_pad_top(&style_cont_240, 80);
        lv_style_set_pad_bottom(&style_cont_240, 80);
        lv_style_set_layout(&style_flex, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_flex, LV_FLEX_FLOW_ROW_WRAP);
        lv_style_set_pad_column(&style_flex, 15);
        lv_style_set_pad_row(&style_flex, 10);
        lv_style_set_pad_hor(&style_pad_rect, 18);
        lv_style_set_pad_hor(&style_pad_rect_360, 14);
        lv_style_set_pad_hor(&style_pad_rect_240, 10);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "phone_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * main_about_list = wd_list_create(lv_obj_0);
        lv_obj_set_name(main_about_list, "main_about_list");
        wd_list_bind_screen(main_about_list, &sb_screen_size);
        lv_obj_t * wd_list_title_0 = wd_list_get_title(main_about_list);
        lv_obj_set_height(wd_list_title_0, LV_SIZE_CONTENT);
        lv_obj_t * hs_title_pill_0 = hs_title_pill_create(wd_list_title_0, "phone_link");
        lv_obj_set_width(hs_title_pill_0, LV_SIZE_CONTENT);
        lv_obj_set_align(hs_title_pill_0, LV_ALIGN_CENTER);
        lv_obj_set_style_max_width(hs_title_pill_0, lv_pct(100), 0);

        lv_obj_t * wd_list_container_0 = wd_list_get_container(main_about_list);
        lv_obj_add_style(wd_list_container_0, &style_cont, 0);
        lv_obj_bind_style(wd_list_container_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_bind_state_if_eq(wd_list_container_0, &sb_screen_type, LV_STATE_USER_1, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect, LV_STATE_USER_1, &sb_screen_size, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_360, LV_STATE_USER_1, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_240, LV_STATE_USER_1, &sb_screen_size, 2);
        lv_obj_t * hs_card_0 = hs_card_create(wd_list_container_0);
        hs_info_sb_create(hs_card_0, "Label", "status", &sb_system_connection_str, "%s");

        lv_obj_t * hs_info_sb_1 = hs_info_sb_create(hs_card_0, "Label", "last_sync", &sb_phone_last_sync, "%s");
        lv_obj_bind_flag_if_eq(hs_info_sb_1, &sb_system_connection, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_card_1 = hs_card_create(wd_list_container_0);
        hs_info_sb_sb_create(hs_card_1, &sb_phone_manufacturer, &sb_phone_model, 0);

        hs_line_create(hs_card_1);

        hs_info_sb_create(hs_card_1, "SDK", "", &sb_phone_sdk, "%d");

        hs_line_create(hs_card_1);

        hs_bar_create(hs_card_1, "Brightness", "battery", &sb_phone_battery);

        hs_info_sb_create(hs_card_1, "Label", "charging", &sb_phone_charging_str, "%s");

        lv_obj_t * hs_card_2 = hs_card_create(wd_list_container_0);
        hs_info_create(hs_card_2, "Chronos", "", "App");

        hs_line_create(hs_card_2);

        hs_info_sb_create(hs_card_2, "Label", "version", &sb_chronos_app_version, "%s");

        hs_info_sb_create(hs_card_2, "Label", "code", &sb_chronos_app_code, "%d");

        lv_obj_t * hs_card_3 = hs_card_create(wd_list_container_0);
        lv_obj_bind_flag_if_eq(hs_card_3, &sb_system_connection, LV_OBJ_FLAG_HIDDEN, 0);
        lv_obj_t * hs_button_0 = hs_button_create(hs_card_3, "Circular Scroll", "find_phone", COLOR_PRIMARY);
        lv_obj_set_flag(hs_button_0, LV_OBJ_FLAG_CHECKABLE, true);
        lv_obj_set_style_bg_color(hs_button_0, COLOR_BUTTON_PRIMARY, 0);

        hs_space_create(wd_list_container_0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

