/**
 * @file info_dialog_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "info_dialog_gen.h"
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

lv_obj_t * info_dialog_create(lv_obj_t * parent, const char * title, const char * info, const char * positive_btn, const char * negative_btn)
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
        lv_style_set_pad_ver(&style_base, 100);
        lv_style_set_pad_hor(&style_base, 50);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x383838));
        lv_style_set_bg_opa(&style_base, 150);
        lv_style_set_pad_ver(&style_pad_360, 80);
        lv_style_set_pad_hor(&style_pad_360, 40);
        lv_style_set_pad_ver(&style_pad_240, 60);
        lv_style_set_pad_hor(&style_pad_240, 30);
        lv_style_set_pad_hor(&style_pad_rect, 18);
        lv_style_set_pad_hor(&style_pad_rect_360, 14);
        lv_style_set_pad_hor(&style_pad_rect_240, 10);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "info_dialog_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_pad_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_240, 0, &sb_screen_size, 2);
        lv_obj_bind_state_if_eq(lv_obj_0, &sb_screen_type, LV_STATE_USER_1, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_rect, LV_STATE_USER_1, &sb_screen_size, 0);
        lv_obj_bind_style(lv_obj_0, &style_pad_rect_360, LV_STATE_USER_1, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_pad_rect_240, LV_STATE_USER_1, &sb_screen_size, 2);
        lv_obj_t * hs_card_0 = hs_card_create(lv_obj_0);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_card_0);
        lv_obj_set_width(hs_text_normal_0, lv_pct(100));
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(hs_text_normal_0, title);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_card_0);
        lv_obj_set_width(hs_text_small_0, lv_pct(100));
        lv_label_set_text(hs_text_small_0, info);

        lv_obj_t * hs_column_0 = hs_column_create(hs_card_0);
        lv_obj_set_style_pad_row(hs_column_0, 10, 0);
        lv_obj_t * button_positive = hs_button_create(hs_column_0, positive_btn, "", COLOR_PRIMARY);
        lv_obj_set_name(button_positive, "button_positive");
        lv_obj_set_style_bg_color(button_positive, COLOR_BUTTON_PRIMARY, 0);

        lv_obj_t * button_negative = hs_button_create(hs_column_0, negative_btn, "", COLOR_DANGER);
        lv_obj_set_name(button_negative, "button_negative");
        lv_obj_set_style_bg_color(button_negative, COLOR_BUTTON_DANGER, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

