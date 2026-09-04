/**
 * @file timer_countdown_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "timer_countdown_gen.h"
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

lv_obj_t * timer_countdown_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_list;
    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_list);
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, lv_pct(100));
        lv_style_set_bg_color(&style_list, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_list, 255);
        lv_style_set_pad_ver(&style_cont, 30);
        lv_style_set_pad_row(&style_cont, 18);
        lv_style_set_pad_ver(&style_cont_360, 20);
        lv_style_set_pad_row(&style_cont_360, 8);
        lv_style_set_pad_ver(&style_cont_240, 12);
        lv_style_set_pad_row(&style_cont_240, 4);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "timer_countdown_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_label_set_text(hs_text_normal_0, "00:00:00");
        lv_obj_set_align(hs_text_normal_0, LV_ALIGN_CENTER);

        lv_obj_t * hs_column_0 = hs_column_create(lv_obj_0);
        lv_obj_set_style_flex_cross_place(hs_column_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_align(hs_column_0, LV_ALIGN_BOTTOM_MID);
        lv_obj_set_style_bg_color(hs_column_0, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(hs_column_0, 200, 0);
        lv_obj_add_style(hs_column_0, &style_cont, 0);
        lv_obj_bind_style(hs_column_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_column_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_row_0 = hs_row_create(hs_column_0);
        lv_obj_set_style_flex_main_place(hs_row_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(hs_row_0, 10, 0);
        lv_obj_t * hs_container_0 = hs_container_create(hs_row_0);
        lv_obj_set_flex_grow(hs_container_0, 1);
        lv_obj_bind_flag_if_eq(hs_container_0, &sb_screen_type, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_button_icon_0 = hs_button_icon_create(hs_row_0, icon_timer_cancel, 0, &sb_placeholder);
        lv_obj_set_flex_grow(hs_button_icon_0, 2);

        lv_obj_t * hs_button_icon_1 = hs_button_icon_create(hs_row_0, icon_music_play_32, 0, &sb_placeholder);
        lv_obj_set_flex_grow(hs_button_icon_1, 2);

        lv_obj_t * hs_container_1 = hs_container_create(hs_row_0);
        lv_obj_set_flex_grow(hs_container_1, 1);
        lv_obj_bind_flag_if_eq(hs_container_1, &sb_screen_type, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_column_0);
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

