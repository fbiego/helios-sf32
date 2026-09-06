/**
 * @file mpesa_item_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "mpesa_item_gen.h"
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

lv_obj_t * mpesa_item_create(lv_obj_t * parent, const char * tx_name, const char * tx_time, const char * tx_amount, bool tx_in)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_container;
    static lv_style_t style_cont_pressed;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;
    static lv_style_t style_text_cont;
    static lv_style_t style_text_cont_360;
    static lv_style_t style_text_cont_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_container);
        lv_style_init(&style_cont_pressed);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);
        lv_style_init(&style_text_cont);
        lv_style_init(&style_text_cont_360);
        lv_style_init(&style_text_cont_240);

        lv_style_set_flex_flow(&style_container, LV_FLEX_FLOW_ROW);
        lv_style_set_pad_column(&style_container, 10);
        lv_style_set_flex_cross_place(&style_container, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&style_container, LV_FLEX_ALIGN_CENTER);
        lv_style_set_bg_color(&style_cont_pressed, lv_color_hex(0x545454));
        lv_style_set_pad_column(&style_cont_360, 8);
        lv_style_set_pad_column(&style_cont_240, 6);
        lv_style_set_pad_row(&style_text_cont, 3);
        lv_style_set_pad_row(&style_text_cont_360, 2);
        lv_style_set_pad_row(&style_text_cont_240, 1);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * hs_card_0 = hs_card_create(parent);
        lv_obj_set_name_static(hs_card_0, "mpesa_item_#");

        lv_obj_add_style(hs_card_0, &style_container, 0);
        lv_obj_add_style(hs_card_0, &style_cont_pressed, LV_STATE_PRESSED);
        lv_obj_bind_style(hs_card_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_card_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_column_0 = hs_column_create(hs_card_0);
        lv_obj_set_flex_grow(hs_column_0, 1);
        lv_obj_set_flag(hs_column_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(hs_column_0, &style_text_cont, 0);
        lv_obj_bind_style(hs_column_0, &style_text_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_column_0, &style_text_cont_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_column_0);
        lv_obj_set_width(hs_text_small_0, lv_pct(100));
        lv_label_set_long_mode(hs_text_small_0, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_label_set_text(hs_text_small_0, tx_name);

        lv_obj_t * hs_text_small_1 = hs_text_small_create(hs_column_0);
        lv_obj_set_width(hs_text_small_1, lv_pct(100));
        lv_label_set_long_mode(hs_text_small_1, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_label_set_text(hs_text_small_1, tx_time);
        lv_obj_set_style_text_color(hs_text_small_1, lv_color_hex(0xbbbbbb), 0);

        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_card_0);
        lv_label_set_text(hs_text_normal_0, tx_amount);
        lv_obj_set_style_text_color(hs_text_normal_0, tx_in ? lv_color_hex(0x237028) : lv_color_hex(0xD82628), 0);

        the_root = hs_card_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

