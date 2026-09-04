/**
 * @file notification_item_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "notification_item_gen.h"
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

lv_obj_t * notification_item_create(lv_obj_t * parent, const void * icon, const char * title, const char * time, const char * message)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;
    static lv_style_t style_title_cont;
    static lv_style_t style_text_cont;
    static lv_style_t style_text;
    static lv_style_t style_text_360;
    static lv_style_t style_text_240;
    static lv_style_t style_text_focused;
    static lv_style_t style_pressed;
    static lv_style_t style_focused;
    static lv_style_t style_bottom_cont;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);
        lv_style_init(&style_title_cont);
        lv_style_init(&style_text_cont);
        lv_style_init(&style_text);
        lv_style_init(&style_text_360);
        lv_style_init(&style_text_240);
        lv_style_init(&style_text_focused);
        lv_style_init(&style_pressed);
        lv_style_init(&style_focused);
        lv_style_init(&style_bottom_cont);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, LV_SIZE_CONTENT);
        lv_style_set_text_color(&style_base, lv_color_hex3(0xfff));
        lv_style_set_radius(&style_base, 30);
        lv_style_set_bg_color(&style_base, lv_color_hex(0xb7b7b7));
        lv_style_set_bg_opa(&style_base, 20);
        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_base, LV_FLEX_FLOW_COLUMN);
        lv_style_set_pad_all(&style_base, 10);
        lv_style_set_pad_row(&style_base, 5);
        lv_style_set_radius(&style_base_360, 25);
        lv_style_set_pad_all(&style_base_360, 8);
        lv_style_set_pad_row(&style_base_360, 3);
        lv_style_set_radius(&style_base_240, 20);
        lv_style_set_pad_all(&style_base_240, 6);
        lv_style_set_pad_row(&style_base_240, 0);
        lv_style_set_width(&style_title_cont, lv_pct(100));
        lv_style_set_height(&style_title_cont, LV_SIZE_CONTENT);
        lv_style_set_layout(&style_title_cont, LV_LAYOUT_FLEX);
        lv_style_set_pad_column(&style_title_cont, 10);
        lv_style_set_pad_hor(&style_title_cont, 5);
        lv_style_set_pad_top(&style_title_cont, 7);
        lv_style_set_flex_grow(&style_text_cont, 1);
        lv_style_set_height(&style_text_cont, LV_SIZE_CONTENT);
        lv_style_set_layout(&style_text_cont, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_text_cont, LV_FLEX_FLOW_COLUMN);
        lv_style_set_pad_top(&style_text_cont, 0);
        lv_style_set_pad_row(&style_text_cont, 0);
        lv_style_set_width(&style_text, lv_pct(100));
        lv_style_set_height(&style_text, 30);
        lv_style_set_pad_all(&style_text, 5);
        lv_style_set_height(&style_text_360, 29);
        lv_style_set_height(&style_text_240, 26);
        lv_style_set_height(&style_text_focused, LV_SIZE_CONTENT);
        lv_style_set_bg_color(&style_pressed, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_pressed, 127);
        lv_style_set_height(&style_focused, LV_SIZE_CONTENT);
        lv_style_set_bg_opa(&style_focused, 40);
        lv_style_set_height(&style_bottom_cont, 10);
        lv_style_set_bg_color(&style_bottom_cont, lv_color_hex(0xb7b7b7));
        lv_style_set_bg_opa(&style_bottom_cont, 255);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "notification_item_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLL_ON_FOCUS, true);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_STATE_TRICKLE, true);

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_add_style(lv_obj_0, &style_focused, LV_STATE_FOCUSED);
        lv_obj_bind_style(lv_obj_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_bind_style(lv_obj_0, &style_focused, 0, &sb_focusable, 1);
        lv_obj_t * hs_container_0 = hs_container_create(lv_obj_0);
        lv_obj_set_flag(hs_container_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(hs_container_0, &style_title_cont, 0);
        lv_obj_t * wd_image_0 = wd_image_create(hs_container_0);
        wd_image_set_src(wd_image_0, icon);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 200);
        wd_image_set_scale_2(wd_image_0, 140);
        wd_image_set_size_1(wd_image_0, 50);
        wd_image_set_size_2(wd_image_0, 35);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);

        lv_obj_t * hs_container_1 = hs_container_create(hs_container_0);
        lv_obj_set_flag(hs_container_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(hs_container_1, &style_text_cont, 0);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_container_1);
        lv_label_set_text(hs_text_normal_0, title);
        lv_obj_set_width(hs_text_normal_0, lv_pct(100));
        lv_label_set_long_mode(hs_text_normal_0, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_container_1);
        lv_label_set_text(hs_text_small_0, time);
        lv_obj_set_width(hs_text_small_0, lv_pct(100));

        lv_obj_t * hs_text_small_1 = hs_text_small_create(lv_obj_0);
        lv_label_set_text(hs_text_small_1, message);
        lv_obj_add_style(hs_text_small_1, &style_text, 0);
        lv_obj_bind_style(hs_text_small_1, &style_text_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_text_small_1, &style_text_240, 0, &sb_screen_size, 2);
        lv_obj_add_style(hs_text_small_1, &style_text_focused, LV_STATE_FOCUSED);
        lv_obj_bind_style(hs_text_small_1, &style_text_focused, 0, &sb_focusable, 1);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

