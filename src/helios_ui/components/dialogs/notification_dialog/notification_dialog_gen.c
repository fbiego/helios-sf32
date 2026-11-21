/**
 * @file notification_dialog_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "notification_dialog_gen.h"
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

lv_obj_t * notification_dialog_create(lv_obj_t * parent)
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

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, lv_pct(100));
        lv_style_set_pad_ver(&style_base, 12);
        lv_style_set_pad_hor(&style_base, 20);
        lv_style_set_pad_row(&style_base, 10);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_base, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_track_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_ver(&style_pad_360, 10);
        lv_style_set_pad_hor(&style_pad_360, 20);
        lv_style_set_pad_row(&style_pad_360, 6);
        lv_style_set_pad_ver(&style_pad_240, 8);
        lv_style_set_pad_hor(&style_pad_240, 10);
        lv_style_set_pad_row(&style_pad_240, 2);
        lv_style_set_pad_hor(&style_pad_rect, 4);
        lv_style_set_pad_hor(&style_pad_rect_360, 14);
        lv_style_set_pad_hor(&style_pad_rect_240, 0);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "notification_dialog_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_pad_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_240, 0, &sb_screen_size, 2);
        lv_obj_bind_state_if_eq(lv_obj_0, &sb_screen_type, LV_STATE_USER_1, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_rect, LV_STATE_USER_1, &sb_screen_size, 0);
        lv_obj_bind_style(lv_obj_0, &style_pad_rect_360, LV_STATE_USER_1, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_rect_240, LV_STATE_USER_1, &sb_screen_size, 2);
        lv_obj_t * icon = wd_image_create(lv_obj_0);
        lv_obj_set_name(icon, "icon");
        lv_obj_set_flag(icon, LV_OBJ_FLAG_CLICKABLE, false);
        lv_image_set_src(icon, icon_nt_wechat);
        wd_image_set_scale_0(icon, 256);
        wd_image_set_scale_1(icon, 190);
        wd_image_set_scale_2(icon, 120);
        wd_image_set_size_1(icon, 55);
        wd_image_set_size_2(icon, 30);
        wd_image_bind_scale(icon, &sb_screen_size);

        lv_obj_t * title = hs_text_normal_create(lv_obj_0);
        lv_obj_set_name(title, "title");
        lv_obj_set_width(title, lv_pct(80));
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(title, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_label_set_text(title, "Lorem ipsum");

        lv_obj_t * hs_container_0 = hs_container_create(lv_obj_0);
        lv_obj_set_width(hs_container_0, lv_pct(90));
        lv_obj_set_flex_grow(hs_container_0, 1);
        lv_obj_set_style_pad_bottom(hs_container_0, 60, 0);
        lv_obj_t * message = hs_text_small_create(hs_container_0);
        lv_obj_set_name(message, "message");
        lv_obj_set_width(message, lv_pct(100));
        lv_label_set_text(message, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

        lv_obj_t * time = hs_text_small_create(lv_obj_0);
        lv_obj_set_name(time, "time");
        lv_label_set_text(time, "13:45");
        lv_obj_set_style_text_color(time, lv_color_hex(0x8d8d8d), 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

