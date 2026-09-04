/**
 * @file navigation_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "navigation_gen.h"
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

lv_obj_t * navigation_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_pad;
    static lv_style_t style_pad_360;
    static lv_style_t style_pad_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_pad);
        lv_style_init(&style_pad_360);
        lv_style_init(&style_pad_240);

        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_base, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_main_place(&style_base, LV_FLEX_ALIGN_SPACE_EVENLY);
        lv_style_set_flex_track_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_ver(&style_pad, 80);
        lv_style_set_pad_ver(&style_pad_360, 60);
        lv_style_set_pad_ver(&style_pad_240, 40);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "navigation_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * wd_list_0 = wd_list_create(lv_obj_0);
        lv_obj_t * wd_list_container_0 = wd_list_get_container(wd_list_0);
        lv_obj_add_style(wd_list_container_0, &style_base, 0);
        lv_obj_add_style(wd_list_container_0, &style_pad, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(wd_list_container_0);
        lv_label_set_text(hs_text_normal_0, "Navigation");
        lv_obj_set_width(hs_text_normal_0, lv_pct(90));
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_bind_text(hs_text_normal_0, &sb_nav_text, NULL);

        lv_obj_t * wd_image_0 = wd_image_create(wd_list_container_0);
        wd_image_set_scale_0(wd_image_0, 500);
        wd_image_set_scale_1(wd_image_0, 385);
        wd_image_set_scale_2(wd_image_0, 256);
        wd_image_set_size_0(wd_image_0, 94);
        wd_image_set_size_1(wd_image_0, 72);
        wd_image_set_size_2(wd_image_0, 48);
        wd_image_bind_src(wd_image_0, &sb_nav_icon);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);
        lv_obj_set_style_image_recolor(wd_image_0, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_image_recolor_opa(wd_image_0, 255, 0);

        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(wd_list_container_0);
        lv_label_set_text(hs_text_normal_1, "ESP32");
        lv_obj_set_width(hs_text_normal_1, lv_pct(90));
        lv_obj_set_style_text_align(hs_text_normal_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_bind_text(hs_text_normal_1, &sb_nav_title, NULL);

        lv_obj_t * hs_text_small_0 = hs_text_small_create(wd_list_container_0);
        lv_label_set_text(hs_text_small_0, "Avaialble on Chronos v3.7.5+");
        lv_obj_set_width(hs_text_small_0, lv_pct(90));
        lv_obj_set_style_text_align(hs_text_small_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_bind_text(hs_text_small_0, &sb_nav_directions, NULL);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

