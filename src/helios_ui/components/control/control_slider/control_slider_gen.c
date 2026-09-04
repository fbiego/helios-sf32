/**
 * @file control_slider_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "control_slider_gen.h"
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

lv_obj_t * control_slider_create(lv_obj_t * parent, const void * icon_left, const void * icon_right)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_slider;
    static lv_style_t style_slider_360;
    static lv_style_t style_slider_240;
    static lv_style_t style_indicator;
    static lv_style_t style_disabled;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_slider);
        lv_style_init(&style_slider_360);
        lv_style_init(&style_slider_240);
        lv_style_init(&style_indicator);
        lv_style_init(&style_disabled);

        lv_style_set_width(&style_slider, 235);
        lv_style_set_height(&style_slider, 110);
        lv_style_set_radius(&style_slider, 55);
        lv_style_set_bg_color(&style_slider, lv_color_hex(0x282828));
        lv_style_set_bg_opa(&style_slider, 255);
        lv_style_set_width(&style_slider_360, 190);
        lv_style_set_height(&style_slider_360, 90);
        lv_style_set_width(&style_slider_240, 130);
        lv_style_set_height(&style_slider_240, 60);
        lv_style_set_bg_color(&style_indicator, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_indicator, 255);
        lv_style_set_bg_color(&style_disabled, lv_color_hex(0x898989));

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_slider_0 = lv_slider_create(parent);
        lv_obj_set_name_static(lv_slider_0, "control_slider_#");
        lv_obj_set_flag(lv_slider_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_slider_0, LV_OBJ_FLAG_EVENT_BUBBLE, true);
        lv_obj_set_flag(lv_slider_0, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);

        lv_obj_remove_style_all(lv_slider_0);
        lv_obj_add_style(lv_slider_0, &style_slider, 0);
        lv_obj_add_style(lv_slider_0, &style_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_slider_0, &style_disabled, LV_STATE_DISABLED);
        lv_obj_bind_style(lv_slider_0, &style_slider_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_slider_0, &style_slider_240, 0, &sb_screen_size, 2);
        lv_obj_t * wd_image_0 = wd_image_create(lv_slider_0);
        lv_obj_set_flag(wd_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(wd_image_0, LV_ALIGN_LEFT_MID);
        wd_image_set_src(wd_image_0, icon_left);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 190);
        wd_image_set_scale_2(wd_image_0, 130);
        wd_image_set_size_1(wd_image_0, 60);
        wd_image_set_size_2(wd_image_0, 40);
        wd_image_set_x_0(wd_image_0, 24);
        wd_image_set_x_1(wd_image_0, 14);
        wd_image_set_x_2(wd_image_0, 8);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);
        lv_obj_set_style_blend_mode(wd_image_0, LV_BLEND_MODE_DIFFERENCE, 0);
        lv_obj_set_style_image_recolor(wd_image_0, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_image_recolor_opa(wd_image_0, 255, 0);

        lv_obj_t * wd_image_1 = wd_image_create(lv_slider_0);
        lv_obj_set_flag(wd_image_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(wd_image_1, LV_ALIGN_RIGHT_MID);
        wd_image_set_src(wd_image_1, icon_right);
        wd_image_set_scale_0(wd_image_1, 256);
        wd_image_set_scale_1(wd_image_1, 190);
        wd_image_set_scale_2(wd_image_1, 130);
        wd_image_set_size_1(wd_image_1, 60);
        wd_image_set_size_2(wd_image_1, 40);
        wd_image_set_x_0(wd_image_1, -24);
        wd_image_set_x_1(wd_image_1, -14);
        wd_image_set_x_2(wd_image_1, -8);
        wd_image_bind_scale(wd_image_1, &sb_screen_size);
        lv_obj_set_style_blend_mode(wd_image_1, LV_BLEND_MODE_DIFFERENCE, 0);
        lv_obj_set_style_image_recolor(wd_image_1, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_image_recolor_opa(wd_image_1, 255, 0);

        the_root = lv_slider_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

