/**
 * @file control_panel_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "control_panel_gen.h"
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

lv_obj_t * control_panel_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_control;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;
    static lv_style_t style_grid;
    static lv_style_t style_bar;
    static lv_style_t style_bar_pressed;
    static lv_style_t style_min_pad;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_control);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);
        lv_style_init(&style_grid);
        lv_style_init(&style_bar);
        lv_style_init(&style_bar_pressed);
        lv_style_init(&style_min_pad);

        lv_style_set_width(&style_control, lv_pct(100));
        lv_style_set_height(&style_control, lv_pct(100));
        lv_style_set_bg_color(&style_control, lv_color_hex(0x0e0e0e));
        lv_style_set_bg_opa(&style_control, 255);
        lv_style_set_pad_hor(&style_cont_360, 35);
        lv_style_set_pad_row(&style_cont_360, 10);
        lv_style_set_pad_column(&style_cont_360, 10);
        lv_style_set_pad_top(&style_cont_360, 60);
        lv_style_set_pad_bottom(&style_cont_360, 80);
        lv_style_set_pad_hor(&style_cont_240, 20);
        lv_style_set_pad_row(&style_cont_240, 10);
        lv_style_set_pad_column(&style_cont_240, 10);
        lv_style_set_pad_top(&style_cont_240, 40);
        lv_style_set_pad_bottom(&style_cont_240, 60);
        lv_style_set_width(&style_grid, lv_pct(100));
        lv_style_set_height(&style_grid, lv_pct(100));
        lv_style_set_align(&style_grid, LV_ALIGN_CENTER);
        lv_style_set_flex_main_place(&style_grid, LV_FLEX_ALIGN_CENTER);
        lv_style_set_border_width(&style_grid, 0);
        lv_style_set_pad_top(&style_grid, 70);
        lv_style_set_pad_bottom(&style_grid, 100);
        lv_style_set_pad_hor(&style_grid, 3);
        lv_style_set_width(&style_bar, lv_pct(100));
        lv_style_set_height(&style_bar, lv_pct(15));
        lv_style_set_align(&style_bar, LV_ALIGN_BOTTOM_MID);
        lv_style_set_bg_color(&style_bar_pressed, lv_color_hex3(0xfff));
        lv_style_set_bg_opa(&style_bar_pressed, 100);
        lv_style_set_pad_hor(&style_min_pad, 20);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "control_panel_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_control, 0);
        lv_obj_t * control_list = wd_list_create(lv_obj_0);
        lv_obj_set_name(control_list, "control_list");
        wd_list_bind_screen(control_list, &sb_screen_size);
        lv_obj_add_style(control_list, &style_control, 0);
        lv_obj_t * wd_list_container_0 = wd_list_get_container(control_list);
        lv_obj_set_flag(wd_list_container_0, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_add_style(wd_list_container_0, &style_grid, 0);
        lv_obj_bind_style(wd_list_container_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_bind_style(wd_list_container_0, &style_min_pad, 0, &sb_screen_type, 1);
        lv_obj_t * control_group_0 = control_group_create(wd_list_container_0);
        lv_obj_t * control_slider_0 = control_slider_create(control_group_0, icon_brightness_full, icon_empty);
        lv_slider_bind_value(control_slider_0, &sb_screen_brightness);

        control_bar_create(control_group_0, &sb_battery_percent, "%d%%", icon_empty, false);

        lv_obj_t * control_group_1 = control_group_create(wd_list_container_0);
        lv_obj_t * control_slider_1 = control_slider_create(control_group_1, icon_volume_high, icon_empty);
        lv_slider_bind_value(control_slider_1, &sb_sound_volume);

        lv_obj_t * control_switch_0 = control_switch_create(control_group_1, icon_connect);
        lv_obj_set_flag(control_switch_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_bind_checked(control_switch_0, &sb_system_connection);

        lv_obj_t * music_info_0 = music_info_create(wd_list_container_0);
        lv_obj_bind_flag_if_eq(music_info_0, &sb_system_connection, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * control_music_0 = control_music_create(wd_list_container_0);
        lv_obj_bind_flag_if_eq(control_music_0, &sb_system_connection, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * control_group_2 = control_group_create(wd_list_container_0);
        lv_obj_t * control_switch_1 = control_switch_create(control_group_2, icon_list);
        lv_obj_bind_checked(control_switch_1, &sb_app_list_mode);

        lv_obj_t * control_switch_2 = control_switch_create(control_group_2, icon_vibration);
        lv_obj_bind_checked(control_switch_2, &sb_list_circular_mode);

        lv_obj_t * control_switch_3 = control_switch_create(control_group_2, icon_dnd);
        lv_obj_bind_checked(control_switch_3, &sb_focusable);

        lv_obj_t * drag_control = lv_obj_create(lv_obj_0);
        lv_obj_set_name(drag_control, "drag_control");
        lv_obj_set_flag(drag_control, LV_OBJ_FLAG_GESTURE_BUBBLE, false);
        lv_obj_remove_style_all(drag_control);
        lv_obj_add_style(drag_control, &style_bar, 0);
        lv_obj_add_style(drag_control, &style_bar_pressed, LV_STATE_PRESSED);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

