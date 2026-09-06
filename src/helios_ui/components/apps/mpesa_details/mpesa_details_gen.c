/**
 * @file mpesa_details_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "mpesa_details_gen.h"
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

lv_obj_t * mpesa_details_create(lv_obj_t * parent, const char * tx_name, const char * tx_time, const char * tx_amount, const char * tx_account, const char * tx_id, bool tx_in, const char * tx_fee)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;
    static lv_style_t style_col;
    static lv_style_t style_pad_row;
    static lv_style_t style_pad_row_360;
    static lv_style_t style_pad_row_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);
        lv_style_init(&style_col);
        lv_style_init(&style_pad_row);
        lv_style_init(&style_pad_row_360);
        lv_style_init(&style_pad_row_240);

        lv_style_set_bg_color(&style_cont, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_cont, 255);
        lv_style_set_width(&style_cont, lv_pct(100));
        lv_style_set_height(&style_cont, lv_pct(100));
        lv_style_set_flex_cross_place(&style_cont, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&style_cont, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_main_place(&style_cont, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_row(&style_cont, 12);
        lv_style_set_pad_row(&style_cont_360, 8);
        lv_style_set_pad_row(&style_cont_240, 4);
        lv_style_set_flex_cross_place(&style_col, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&style_col, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_row(&style_pad_row, 8);
        lv_style_set_pad_row(&style_pad_row_360, 6);
        lv_style_set_pad_row(&style_pad_row_240, 4);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        char eval_buf_1[LV_XML_EVAL_STRING_BUF_SIZE];

        lv_obj_t * hs_column_0 = hs_column_create(parent);
        lv_obj_set_name_static(hs_column_0, "mpesa_details_#");
        lv_obj_set_flag(hs_column_0, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_add_style(hs_column_0, &style_cont, 0);
        lv_obj_bind_style(hs_column_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_column_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_column_1 = hs_column_create(hs_column_0);
        lv_obj_add_style(hs_column_1, &style_col, 0);
        lv_obj_bind_style(hs_column_1, &style_pad_row, 0, &sb_screen_size, 0);
        lv_obj_bind_style(hs_column_1, &style_pad_row_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_column_1, &style_pad_row_240, 0, &sb_screen_size, 2);
        lv_obj_t * wd_image_0 = wd_image_create(hs_column_1);
        wd_image_set_src(wd_image_0, icon_mpesa_watch);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 190);
        wd_image_set_scale_2(wd_image_0, 120);
        wd_image_set_size_1(wd_image_0, 55);
        wd_image_set_size_2(wd_image_0, 30);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);
        lv_obj_set_style_image_recolor(wd_image_0, tx_in ? lv_color_hex(0x237028) : lv_color_hex(0xD82628), 0);
        lv_obj_set_style_image_recolor_opa(wd_image_0, 255, 0);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(hs_column_1);
        lv_label_set_text(hs_text_small_0, tx_in ? "Received" : "Sent");
        lv_obj_set_style_text_color(hs_text_small_0, tx_in ? lv_color_hex(0x237028) : lv_color_hex(0xD82628), 0);
        lv_obj_set_style_text_opa(hs_text_small_0, 255, 0);

        lv_obj_t * hs_card_0 = hs_card_create(hs_column_0);
        lv_obj_set_width(hs_card_0, LV_SIZE_CONTENT);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_card_0);
        lv_label_set_text(hs_text_normal_0, tx_id);
        lv_obj_set_style_text_letter_space(hs_text_normal_0, 2, 0);

        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(hs_column_0);
        lv_label_set_text(hs_text_normal_1, tx_amount);

        lv_obj_t * hs_text_small_1 = hs_text_small_create(hs_column_0);
        lv_obj_set_width(hs_text_small_1, lv_pct(90));
        lv_obj_set_style_text_align(hs_text_small_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(hs_text_small_1, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_label_set_text(hs_text_small_1, tx_name);

        lv_obj_t * hs_column_2 = hs_column_create(hs_column_0);
        lv_obj_add_style(hs_column_2, &style_col, 0);
        lv_obj_bind_style(hs_column_2, &style_pad_row, 0, &sb_screen_size, 0);
        lv_obj_bind_style(hs_column_2, &style_pad_row_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(hs_column_2, &style_pad_row_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_small_2 = hs_text_small_create(hs_column_2);
        lv_label_set_text(hs_text_small_2, tx_account);
        lv_obj_set_style_text_color(hs_text_small_2, lv_color_hex(0xA2A2A2), 0);

        lv_obj_t * hs_text_small_3 = hs_text_small_create(hs_column_2);
        lv_label_set_text(hs_text_small_3, tx_time);
        lv_obj_set_style_text_color(hs_text_small_3, lv_color_hex(0xA2A2A2), 0);

        lv_obj_t * hs_text_small_4 = hs_text_small_create(hs_column_2);
        lv_obj_set_flag(hs_text_small_4, LV_OBJ_FLAG_HIDDEN, tx_in);
        lv_snprintf(eval_buf_1, sizeof(eval_buf_1), "%s%s", "Fee: ", tx_fee);
        lv_label_set_text(hs_text_small_4, eval_buf_1);
        lv_obj_set_style_text_color(hs_text_small_4, lv_color_hex(0xA2A2A2), 0);

        the_root = hs_column_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

