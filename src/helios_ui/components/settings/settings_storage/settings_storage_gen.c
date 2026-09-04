/**
 * @file settings_storage_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "settings_storage_gen.h"
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

lv_obj_t * settings_storage_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_pad_360;
    static lv_style_t style_pad_240;
    static lv_style_t style_pad_rect;
    static lv_style_t style_pad_rect_360;
    static lv_style_t style_pad_rect_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_pad_360);
        lv_style_init(&style_pad_240);
        lv_style_init(&style_pad_rect);
        lv_style_init(&style_pad_rect_360);
        lv_style_init(&style_pad_rect_240);

        lv_style_set_pad_row(&style_base, 10);
        lv_style_set_pad_ver(&style_base, 120);
        lv_style_set_pad_ver(&style_pad_360, 90);
        lv_style_set_pad_hor(&style_pad_360, 25);
        lv_style_set_pad_ver(&style_pad_240, 60);
        lv_style_set_pad_hor(&style_pad_240, 20);
        lv_style_set_pad_hor(&style_pad_rect, 18);
        lv_style_set_pad_hor(&style_pad_rect_360, 14);
        lv_style_set_pad_hor(&style_pad_rect_240, 10);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * wd_list_0 = wd_list_create(parent);
        lv_obj_set_name_static(wd_list_0, "settings_storage_#");
        wd_list_bind_screen(wd_list_0, &sb_screen_size);

        lv_obj_t * wd_list_title_0 = wd_list_get_title(wd_list_0);
        lv_obj_set_height(wd_list_title_0, LV_SIZE_CONTENT);
        lv_obj_t * hs_title_pill_0 = hs_title_pill_create(wd_list_title_0, "storage");
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
        hs_bar_create(hs_card_0, "Brightness", "system", &sb_battery_percent);

        lv_obj_t * hs_card_1 = hs_card_create(wd_list_container_0);
        hs_bar_create(hs_card_1, "LittleFS", "", &sb_battery_percent);

        hs_line_create(hs_card_1);

        lv_obj_t * hs_column_0 = hs_column_create(hs_card_1);
        lv_obj_set_style_pad_all(hs_column_0, 0, 0);
        hs_info_create(hs_column_0, "Label", "watchfaces", "5");

        hs_info_create(hs_column_0, "Label", "steps", "34");

        hs_info_create(hs_column_0, "Label", "battery", "56");

        hs_line_create(hs_card_1);

        lv_obj_t * hs_button_0 = hs_button_create(hs_card_1, "Circular Scroll", "format", COLOR_DANGER);
        lv_obj_set_style_bg_color(hs_button_0, COLOR_BUTTON_DANGER, 0);

        lv_obj_t * hs_card_2 = hs_card_create(wd_list_container_0);
        hs_bar_create(hs_card_2, "Brightness", "sd_card", &sb_battery_percent);

        hs_line_create(hs_card_2);

        lv_obj_t * hs_button_1 = hs_button_create(hs_card_2, "Circular Scroll", "format", COLOR_DANGER);
        lv_obj_set_style_bg_color(hs_button_1, COLOR_BUTTON_DANGER, 0);

        the_root = wd_list_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

