/**
 * @file control_bar_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "control_bar_gen.h"
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

lv_obj_t * control_bar_create(lv_obj_t * parent, lv_subject_t * bind_subject, const char * bind_fmt, const void * icon, bool hide_text)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_bar;
    static lv_style_t style_bar_360;
    static lv_style_t style_bar_240;
    static lv_style_t style_indicator;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_bar);
        lv_style_init(&style_bar_360);
        lv_style_init(&style_bar_240);
        lv_style_init(&style_indicator);

        lv_style_set_width(&style_bar, 110);
        lv_style_set_height(&style_bar, 110);
        lv_style_set_radius(&style_bar, 55);
        lv_style_set_bg_color(&style_bar, lv_color_hex(0x282828));
        lv_style_set_bg_opa(&style_bar, 255);
        lv_style_set_text_color(&style_bar, lv_color_hex(0xffffff));
        lv_style_set_text_font(&style_bar, NS_Medium_40);
        lv_style_set_width(&style_bar_360, 90);
        lv_style_set_height(&style_bar_360, 90);
        lv_style_set_text_font(&style_bar_360, NS_Medium_30);
        lv_style_set_width(&style_bar_240, 60);
        lv_style_set_height(&style_bar_240, 60);
        lv_style_set_text_font(&style_bar_240, NS_Medium_20);
        lv_style_set_bg_color(&style_indicator, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_indicator, 255);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_bar_0 = lv_bar_create(parent);
        lv_obj_set_name_static(lv_bar_0, "control_bar_#");
        lv_bar_set_orientation(lv_bar_0, LV_BAR_ORIENTATION_VERTICAL);
        lv_bar_bind_value(lv_bar_0, bind_subject);
        lv_obj_set_flag(lv_bar_0, LV_OBJ_FLAG_EVENT_BUBBLE, true);

        lv_obj_remove_style_all(lv_bar_0);
        lv_obj_add_style(lv_bar_0, &style_bar, 0);
        lv_obj_add_style(lv_bar_0, &style_indicator, LV_PART_INDICATOR);
        lv_obj_bind_style(lv_bar_0, &style_bar_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_bar_0, &style_bar_240, 0, &sb_screen_size, 2);
        lv_obj_t * wd_image_0 = wd_image_create(lv_bar_0);
        lv_obj_set_flag(wd_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(wd_image_0, LV_ALIGN_CENTER);
        wd_image_set_src(wd_image_0, icon);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 190);
        wd_image_set_scale_2(wd_image_0, 130);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);
        lv_obj_set_style_blend_mode(wd_image_0, LV_BLEND_MODE_DIFFERENCE, 0);
        lv_obj_set_style_image_recolor(wd_image_0, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_image_recolor_opa(wd_image_0, 255, 0);

        lv_obj_t * lv_label_0 = lv_label_create(lv_bar_0);
        lv_label_bind_text(lv_label_0, bind_subject, bind_fmt);
        lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
        lv_obj_set_style_blend_mode(lv_label_0, LV_BLEND_MODE_DIFFERENCE, 0);
        lv_obj_set_flag(lv_label_0, LV_OBJ_FLAG_HIDDEN, hide_text);
        lv_obj_set_style_pad_top(lv_label_0, 5, 0);

        the_root = lv_bar_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

