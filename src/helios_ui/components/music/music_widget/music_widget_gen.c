/**
 * @file music_widget_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "music_widget_gen.h"
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

lv_obj_t * music_widget_create(lv_obj_t * parent)
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
        lv_style_set_width(&style_cont, lv_pct(100));
        lv_style_set_height(&style_cont, lv_pct(100));
        lv_style_set_pad_ver(&style_cont, 20);
        lv_style_set_layout(&style_cont, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_cont, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_main_place(&style_cont, LV_FLEX_ALIGN_SPACE_BETWEEN);
        lv_style_set_flex_cross_place(&style_cont, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_bottom(&style_cont, 26);
        lv_style_set_pad_ver(&style_cont_360, 15);
        lv_style_set_pad_bottom(&style_cont_360, 23);
        lv_style_set_pad_ver(&style_cont_240, 10);
        lv_style_set_pad_bottom(&style_cont_240, 16);
        lv_style_set_pad_hor(&style_pad_rect, 30);
        lv_style_set_pad_hor(&style_pad_rect_360, 28);
        lv_style_set_pad_hor(&style_pad_rect_240, 15);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "music_widget_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_t * wd_image_0 = wd_image_create(lv_obj_0);
        wd_image_set_src(wd_image_0, img_music_album);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 190);
        wd_image_set_scale_2(wd_image_0, 120);
        wd_image_set_size_1(wd_image_0, 180);
        wd_image_set_size_2(wd_image_0, 120);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);
        wd_image_bind_color(wd_image_0, &sb_music_album_color);
        lv_obj_set_align(wd_image_0, LV_ALIGN_CENTER);
        lv_obj_set_style_image_recolor_opa(wd_image_0, 255, 0);

        lv_obj_t * hs_container_0 = hs_container_create(lv_obj_0);
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
        lv_obj_set_style_pad_row(hs_column_0, 10, 0);
        lv_obj_set_style_bg_color(hs_column_0, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(hs_column_0, 200, 0);
        lv_obj_set_style_pad_bottom(hs_column_0, 10, 0);
        lv_obj_bind_flag_if_eq(hs_column_0, &sb_system_connection, LV_OBJ_FLAG_HIDDEN, 0);
        lv_obj_t * wd_image_1 = wd_image_create(hs_column_0);
        wd_image_set_scale_0(wd_image_1, 256);
        wd_image_set_scale_1(wd_image_1, 190);
        wd_image_set_scale_2(wd_image_1, 120);
        wd_image_set_size_1(wd_image_1, 55);
        wd_image_set_size_2(wd_image_1, 30);
        wd_image_bind_scale(wd_image_1, &sb_screen_size);
        wd_image_bind_src(wd_image_1, &sb_music_icon);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_column_0);
        lv_obj_set_width(hs_text_small_0, lv_pct(90));
        lv_label_bind_text(hs_text_small_0, &sb_music_app, NULL);
        lv_label_set_long_mode(hs_text_small_0, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_style_text_align(hs_text_small_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * hs_column_1 = hs_column_create(hs_container_0);
        lv_obj_set_height(hs_column_1, lv_pct(100));
        lv_obj_set_style_flex_main_place(hs_column_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_cross_place(hs_column_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_track_place(hs_column_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_row(hs_column_1, 10, 0);
        lv_obj_set_style_bg_color(hs_column_1, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(hs_column_1, 200, 0);
        lv_obj_set_style_pad_bottom(hs_column_1, 10, 0);
        lv_obj_bind_flag_if_eq(hs_column_1, &sb_system_connection, LV_OBJ_FLAG_HIDDEN, 1);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_column_1);
        lv_obj_set_width(hs_text_normal_0, lv_pct(90));
        lv_label_set_translation_tag(hs_text_normal_0, "disconnected");
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * hs_text_small_1 = hs_text_small_create(hs_container_0);
        lv_label_bind_text(hs_text_small_1, &sb_time_string, NULL);
        lv_obj_bind_flag_if_eq(hs_text_small_1, &sb_system_connection, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_row_0 = hs_row_create(hs_container_0);
        lv_obj_set_width(hs_row_0, lv_pct(90));
        lv_obj_set_style_flex_cross_place(hs_row_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_bind_flag_if_eq(hs_row_0, &sb_system_connection, LV_OBJ_FLAG_HIDDEN, 0);
        lv_obj_t * hs_button_icon_0 = hs_button_icon_create(hs_row_0, icon_vol_down_32, 0, &sb_placeholder);
        lv_obj_set_flex_grow(hs_button_icon_0, 1);
        lv_subject_increment_dsc_t * subject_increment_event_0 = lv_obj_add_subject_increment_event(hs_button_icon_0, &sb_sound_volume, LV_EVENT_CLICKED, -10);

        lv_obj_t * hs_container_1 = hs_container_create(hs_row_0);
        lv_obj_set_flex_grow(hs_container_1, 3);
        lv_obj_t * simple_bar_0 = simple_bar_create(hs_container_1);
        lv_bar_bind_value(simple_bar_0, &sb_sound_volume);

        lv_obj_t * hs_button_icon_1 = hs_button_icon_create(hs_row_0, icon_vol_up_32, 0, &sb_placeholder);
        lv_obj_set_flex_grow(hs_button_icon_1, 1);
        lv_subject_increment_dsc_t * subject_increment_event_1 = lv_obj_add_subject_increment_event(hs_button_icon_1, &sb_sound_volume, LV_EVENT_CLICKED, 10);

        lv_obj_t * hs_column_2 = hs_column_create(hs_container_0);
        lv_obj_set_style_flex_cross_place(hs_column_2, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_row(hs_column_2, 10, 0);
        lv_obj_set_style_pad_top(hs_column_2, 10, 0);
        lv_obj_set_align(hs_column_2, LV_ALIGN_BOTTOM_MID);
        lv_obj_set_style_bg_color(hs_column_2, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(hs_column_2, 200, 0);
        lv_obj_bind_flag_if_eq(hs_column_2, &sb_system_connection, LV_OBJ_FLAG_HIDDEN, 0);
        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(hs_column_2);
        lv_obj_set_width(hs_text_normal_1, lv_pct(90));
        lv_label_bind_text(hs_text_normal_1, &sb_music_track, NULL);
        lv_label_set_long_mode(hs_text_normal_1, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_style_text_align(hs_text_normal_1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * hs_text_normal_2 = hs_text_normal_create(hs_column_2);
        lv_obj_set_width(hs_text_normal_2, lv_pct(90));
        lv_label_bind_text(hs_text_normal_2, &sb_music_artist, NULL);
        lv_label_set_long_mode(hs_text_normal_2, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_style_text_align(hs_text_normal_2, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t * hs_row_1 = hs_row_create(hs_column_2);
        lv_obj_set_style_flex_main_place(hs_row_1, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_column(hs_row_1, 10, 0);
        lv_obj_t * hs_container_2 = hs_container_create(hs_row_1);
        lv_obj_set_flex_grow(hs_container_2, 1);
        lv_obj_bind_flag_if_eq(hs_container_2, &sb_screen_type, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_button_icon_2 = hs_button_icon_create(hs_row_1, icon_music_previous_32, 0, &sb_placeholder);
        lv_obj_set_flex_grow(hs_button_icon_2, 1);
        lv_obj_add_event_cb(hs_button_icon_2, on_music_control_cb, LV_EVENT_CLICKED, "previous");

        lv_obj_t * hs_button_icon_3 = hs_button_icon_create(hs_row_1, icon_arrow_up, 0, &sb_music_state_icon);
        lv_obj_set_flex_grow(hs_button_icon_3, 1);
        lv_obj_add_event_cb(hs_button_icon_3, on_music_control_cb, LV_EVENT_CLICKED, "toggle");

        lv_obj_t * hs_button_icon_4 = hs_button_icon_create(hs_row_1, icon_music_next_32, 0, &sb_placeholder);
        lv_obj_set_flex_grow(hs_button_icon_4, 1);
        lv_obj_add_event_cb(hs_button_icon_4, on_music_control_cb, LV_EVENT_CLICKED, "next");

        lv_obj_t * hs_container_3 = hs_container_create(hs_row_1);
        lv_obj_set_flex_grow(hs_container_3, 1);
        lv_obj_bind_flag_if_eq(hs_container_3, &sb_screen_type, LV_OBJ_FLAG_HIDDEN, 1);

        lv_obj_t * hs_text_small_2 = hs_text_small_create(hs_column_2);
        lv_label_bind_text(hs_text_small_2, &sb_time_string, NULL);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

