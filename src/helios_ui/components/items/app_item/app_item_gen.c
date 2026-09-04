/**
 * @file app_item_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "app_item_gen.h"
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

lv_obj_t * app_item_create(lv_obj_t * parent, const void * icon, const char * app_name, const char * tag)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_app;
    static lv_style_t style_text;
    static lv_style_t style_text_360;
    static lv_style_t style_text_240;
    static lv_style_t style_app_360;
    static lv_style_t style_app_240;
    static lv_style_t style_grid_mode;
    static lv_style_t style_grid_mode_360;
    static lv_style_t style_grid_mode_240;
    static lv_style_t style_icon;
    static lv_style_t style_icon_360;
    static lv_style_t style_flex;
    static lv_style_t style_pressed;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_app);
        lv_style_init(&style_text);
        lv_style_init(&style_text_360);
        lv_style_init(&style_text_240);
        lv_style_init(&style_app_360);
        lv_style_init(&style_app_240);
        lv_style_init(&style_grid_mode);
        lv_style_init(&style_grid_mode_360);
        lv_style_init(&style_grid_mode_240);
        lv_style_init(&style_icon);
        lv_style_init(&style_icon_360);
        lv_style_init(&style_flex);
        lv_style_init(&style_pressed);

        lv_style_set_width(&style_app, lv_pct(100));
        lv_style_set_height(&style_app, 80);
        lv_style_set_text_font(&style_app, NS_Medium_40);
        lv_style_set_text_color(&style_app, lv_color_hex3(0xfff));
        lv_style_set_radius(&style_app, 40);
        lv_style_set_bg_color(&style_app, lv_color_hex(0xb7b7b7));
        lv_style_set_bg_opa(&style_app, 20);
        lv_style_set_pad_column(&style_app, 12);
        lv_style_set_pad_hor(&style_app, 22);
        lv_style_set_height(&style_text, 50);
        lv_style_set_pad_top(&style_text, 10);
        lv_style_set_height(&style_text_360, 40);
        lv_style_set_pad_top(&style_text_360, 7);
        lv_style_set_height(&style_text_240, 30);
        lv_style_set_pad_top(&style_text_240, 7);
        lv_style_set_height(&style_app_360, 60);
        lv_style_set_text_font(&style_app_360, NS_Medium_30);
        lv_style_set_pad_column(&style_app_360, 10);
        lv_style_set_pad_hor(&style_app_360, 15);
        lv_style_set_height(&style_app_240, 40);
        lv_style_set_text_font(&style_app_240, NS_Medium_20);
        lv_style_set_pad_column(&style_app_240, 10);
        lv_style_set_pad_hor(&style_app_240, 12);
        lv_style_set_width(&style_grid_mode, 110);
        lv_style_set_height(&style_grid_mode, 110);
        lv_style_set_radius(&style_grid_mode, 55);
        lv_style_set_bg_color(&style_grid_mode, lv_color_hex(0xb7b7b7));
        lv_style_set_bg_opa(&style_grid_mode, 20);
        lv_style_set_width(&style_grid_mode_360, 90);
        lv_style_set_height(&style_grid_mode_360, 90);
        lv_style_set_width(&style_grid_mode_240, 60);
        lv_style_set_height(&style_grid_mode_240, 60);
        lv_style_set_align(&style_icon, LV_ALIGN_CENTER);
        lv_style_set_transform_scale_x(&style_icon_360, 200);
        lv_style_set_transform_scale_y(&style_icon_360, 200);
        lv_style_set_layout(&style_flex, LV_LAYOUT_FLEX);
        lv_style_set_pad_hor(&style_flex, 20);
        lv_style_set_pad_column(&style_flex, 20);
        lv_style_set_flex_track_place(&style_flex, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_flex, LV_FLEX_ALIGN_CENTER);
        lv_style_set_bg_color(&style_pressed, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_pressed, 60);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "app_item_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_EVENT_BUBBLE, true);

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_app, 0);
        lv_obj_add_style(lv_obj_0, &style_pressed, LV_STATE_PRESSED);
        lv_obj_add_style(lv_obj_0, &style_pressed, LV_STATE_PRESSED | LV_STATE_USER_1);
        lv_obj_bind_state_if_eq(lv_obj_0, &sb_app_list_mode, LV_STATE_USER_1, 1);
        lv_obj_bind_style(lv_obj_0, &style_flex, 0, &sb_app_list_mode, 0);
        lv_obj_add_style(lv_obj_0, &style_grid_mode, LV_STATE_USER_1);
        lv_obj_bind_style(lv_obj_0, &style_app_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_app_240, 0, &sb_screen_size, 2);
        lv_obj_bind_style(lv_obj_0, &style_grid_mode_360, LV_STATE_USER_1, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_grid_mode_240, LV_STATE_USER_1, &sb_screen_size, 2);
        lv_obj_t * wd_image_0 = wd_image_create(lv_obj_0);
        lv_obj_set_flag(wd_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        wd_image_set_src(wd_image_0, icon);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 190);
        wd_image_set_scale_2(wd_image_0, 120);
        wd_image_set_size_1(wd_image_0, 55);
        wd_image_set_size_2(wd_image_0, 30);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);
        lv_obj_bind_style(wd_image_0, &style_icon, 0, &sb_app_list_mode, 1);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_label_set_text(lv_label_0, app_name);
        lv_obj_set_flex_grow(lv_label_0, 1);
        lv_label_set_long_mode(lv_label_0, LV_LABEL_LONG_MODE_CLIP);
        lv_label_set_translation_tag(lv_label_0, tag);
        lv_obj_add_style(lv_label_0, &style_text, 0);
        lv_obj_bind_style(lv_label_0, &style_text_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_label_0, &style_text_240, 0, &sb_screen_size, 2);
        lv_obj_bind_flag_if_eq(lv_label_0, &sb_app_list_mode, LV_OBJ_FLAG_HIDDEN, 1);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

