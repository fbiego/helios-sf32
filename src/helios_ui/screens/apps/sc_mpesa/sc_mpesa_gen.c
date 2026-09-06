/**
 * @file sc_mpesa_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "sc_mpesa_gen.h"
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

lv_obj_t * sc_mpesa_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;
    static lv_style_t style_pad_grid;
    static lv_style_t style_pad_grid_360;
    static lv_style_t style_pad_grid_240;
    static lv_style_t style_pad_grid_410;
    static lv_style_t style_pad_rect;
    static lv_style_t style_pad_rect_360;
    static lv_style_t style_pad_rect_240;
    static lv_style_t style_pad_row;
    static lv_style_t style_pad_row_360;
    static lv_style_t style_pad_row_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);
        lv_style_init(&style_pad_grid);
        lv_style_init(&style_pad_grid_360);
        lv_style_init(&style_pad_grid_240);
        lv_style_init(&style_pad_grid_410);
        lv_style_init(&style_pad_rect);
        lv_style_init(&style_pad_rect_360);
        lv_style_init(&style_pad_rect_240);
        lv_style_init(&style_pad_row);
        lv_style_init(&style_pad_row_360);
        lv_style_init(&style_pad_row_240);

        lv_style_set_width(&style_cont, 466);
        lv_style_set_height(&style_cont, 466);
        lv_style_set_align(&style_cont, LV_ALIGN_CENTER);
        lv_style_set_border_width(&style_cont, 0);
        lv_style_set_pad_top(&style_cont, 100);
        lv_style_set_pad_bottom(&style_cont, 150);
        lv_style_set_pad_hor(&style_cont, 33);
        lv_style_set_pad_hor(&style_cont_360, 30);
        lv_style_set_pad_top(&style_cont_360, 80);
        lv_style_set_pad_bottom(&style_cont_360, 100);
        lv_style_set_pad_hor(&style_cont_240, 20);
        lv_style_set_pad_row(&style_cont_240, 5);
        lv_style_set_pad_top(&style_cont_240, 60);
        lv_style_set_pad_bottom(&style_cont_240, 80);
        lv_style_set_pad_hor(&style_pad_grid, 54);
        lv_style_set_pad_row(&style_pad_grid, 15);
        lv_style_set_pad_column(&style_pad_grid, 12);
        lv_style_set_pad_hor(&style_pad_grid_360, 30);
        lv_style_set_pad_row(&style_pad_grid_360, 10);
        lv_style_set_pad_column(&style_pad_grid_360, 12);
        lv_style_set_pad_hor(&style_pad_grid_240, 20);
        lv_style_set_pad_row(&style_pad_grid_240, 10);
        lv_style_set_pad_column(&style_pad_grid_240, 10);
        lv_style_set_pad_hor(&style_pad_grid_410, 28);
        lv_style_set_pad_hor(&style_pad_rect, 18);
        lv_style_set_pad_hor(&style_pad_rect_360, 14);
        lv_style_set_pad_hor(&style_pad_rect_240, 10);
        lv_style_set_pad_row(&style_pad_row, 8);
        lv_style_set_pad_row(&style_pad_row_360, 6);
        lv_style_set_pad_row(&style_pad_row_240, 4);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "sc_mpesa_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * wd_list_0 = wd_list_create(lv_obj_0);
        wd_list_bind_screen(wd_list_0, &sb_screen_size);
        lv_obj_t * wd_list_title_0 = wd_list_get_title(wd_list_0);
        lv_obj_set_height(wd_list_title_0, LV_SIZE_CONTENT);
        lv_obj_t * hs_row_0 = hs_row_create(wd_list_title_0);
        lv_obj_set_align(hs_row_0, LV_ALIGN_CENTER);
        lv_obj_set_style_flex_main_place(hs_row_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_row_0);
        lv_label_set_text(hs_text_normal_0, "M");
        lv_obj_set_style_text_color(hs_text_normal_0, lv_color_hex(0x2d942f), 0);

        lv_obj_t * wd_image_0 = wd_image_create(hs_row_0);
        wd_image_set_src(wd_image_0, icon_mpesa_watch_32);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 180);
        wd_image_set_scale_2(wd_image_0, 150);
        wd_image_set_size_1(wd_image_0, 26);
        wd_image_set_size_2(wd_image_0, 15);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);

        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(hs_row_0);
        lv_label_set_text(hs_text_normal_1, "PESA");
        lv_obj_set_style_text_color(hs_text_normal_1, lv_color_hex(0x2d942f), 0);

        lv_obj_t * wd_list_container_0 = wd_list_get_container(wd_list_0);
        lv_obj_bind_state_if_eq(wd_list_container_0, &sb_app_list_mode, LV_STATE_USER_1, 1);
        lv_obj_bind_style(wd_list_container_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_bind_style(wd_list_container_0, &style_pad_grid, LV_STATE_USER_1, &sb_screen_size, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_grid_360, LV_STATE_USER_1, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_grid_240, LV_STATE_USER_1, &sb_screen_size, 2);
        lv_obj_bind_style(wd_list_container_0, &style_pad_grid_410, LV_STATE_USER_1, &sb_screen_width, 410);
        lv_obj_bind_state_if_eq(wd_list_container_0, &sb_screen_type, LV_STATE_USER_2, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect, LV_STATE_USER_2, &sb_screen_size, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_360, LV_STATE_USER_2, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_240, LV_STATE_USER_2, &sb_screen_size, 2);
        lv_obj_t * hs_row_1 = hs_row_create(wd_list_container_0);
        lv_obj_set_style_flex_main_place(hs_row_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_cross_place(hs_row_1, LV_FLEX_ALIGN_START, 0);
        lv_obj_set_style_pad_column(hs_row_1, 10, 0);
        lv_obj_t * hs_text_normal_2 = hs_text_normal_create(hs_row_1);
        lv_label_bind_text(hs_text_normal_2, &sb_mpesa_balance_text, NULL);
        lv_obj_bind_flag_if_eq(hs_text_normal_2, &sb_mpesa_balance_hidden, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_text_normal_3 = hs_text_normal_create(hs_row_1);
        lv_label_set_text(hs_text_normal_3, "Hidden");
        lv_obj_set_style_text_color(hs_text_normal_3, lv_color_hex(0x383838), 0);
        lv_obj_bind_flag_if_eq(hs_text_normal_3, &sb_mpesa_balance_hidden, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * wd_image_1 = wd_image_create(hs_row_1);
        wd_image_bind_src(wd_image_1, &sb_mpesa_hidden_icon);
        wd_image_set_scale_0(wd_image_1, 256);
        wd_image_set_scale_1(wd_image_1, 180);
        wd_image_set_scale_2(wd_image_1, 160);
        wd_image_set_size_1(wd_image_1, 28);
        wd_image_set_size_2(wd_image_1, 20);
        wd_image_bind_scale(wd_image_1, &sb_screen_size);
        lv_obj_set_style_image_recolor(wd_image_1, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_image_recolor_opa(wd_image_1, 255, 0);
        lv_obj_set_flag(wd_image_1, LV_OBJ_FLAG_CLICKABLE, true);
        lv_obj_add_subject_toggle_event(wd_image_1, &sb_mpesa_balance_hidden, LV_EVENT_CLICKED);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(wd_list_container_0);
        lv_label_bind_text(hs_text_small_0, &sb_mpesa_recent_text, NULL);
        lv_obj_set_style_text_color(hs_text_small_0, lv_color_hex(0x888888), 0);

        lv_obj_t * transaction_list = hs_column_create(wd_list_container_0);
        lv_obj_set_name(transaction_list, "transaction_list");
        lv_obj_set_flex_flow(transaction_list, LV_FLEX_FLOW_COLUMN_REVERSE);
        lv_obj_bind_style(transaction_list, &style_pad_row, 0, &sb_screen_size, 0);
        lv_obj_bind_style(transaction_list, &style_pad_row_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(transaction_list, &style_pad_row_240, 0, &sb_screen_size, 2);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

