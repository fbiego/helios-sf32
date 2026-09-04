/**
 * @file control_button_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "control_button_gen.h"
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

lv_obj_t * control_button_create(lv_obj_t * parent, const void * icon)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_button;
    static lv_style_t style_button_360;
    static lv_style_t style_button_240;
    static lv_style_t style_pressed;
    static lv_style_t style_icon;
    static lv_style_t style_disabled;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_button);
        lv_style_init(&style_button_360);
        lv_style_init(&style_button_240);
        lv_style_init(&style_pressed);
        lv_style_init(&style_icon);
        lv_style_init(&style_disabled);

        lv_style_set_width(&style_button, 110);
        lv_style_set_height(&style_button, 110);
        lv_style_set_radius(&style_button, 55);
        lv_style_set_bg_color(&style_button, lv_color_hex(0x282828));
        lv_style_set_bg_opa(&style_button, 255);
        lv_style_set_width(&style_button_360, 90);
        lv_style_set_height(&style_button_360, 90);
        lv_style_set_width(&style_button_240, 60);
        lv_style_set_height(&style_button_240, 60);
        lv_style_set_bg_color(&style_pressed, lv_color_hex(0x0534ff));
        lv_style_set_align(&style_icon, LV_ALIGN_CENTER);
        lv_style_set_image_recolor(&style_icon, lv_color_hex(0xffffff));
        lv_style_set_image_recolor_opa(&style_icon, 255);
        lv_style_set_bg_color(&style_disabled, lv_color_hex(0x3d3c38));

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "control_button_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_EVENT_BUBBLE, true);

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_button, 0);
        lv_obj_add_style(lv_obj_0, &style_pressed, LV_STATE_PRESSED);
        lv_obj_add_style(lv_obj_0, &style_disabled, LV_STATE_DISABLED);
        lv_obj_bind_style(lv_obj_0, &style_button_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_button_240, 0, &sb_screen_size, 2);
        lv_obj_t * wd_image_0 = wd_image_create(lv_obj_0);
        lv_obj_set_flag(wd_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_align(wd_image_0, LV_ALIGN_CENTER);
        wd_image_set_src(wd_image_0, icon);
        wd_image_set_scale_0(wd_image_0, 256);
        wd_image_set_scale_1(wd_image_0, 190);
        wd_image_set_scale_2(wd_image_0, 130);
        wd_image_set_size_1(wd_image_0, 60);
        wd_image_set_size_2(wd_image_0, 40);
        wd_image_bind_scale(wd_image_0, &sb_screen_size);
        lv_obj_set_style_image_recolor(wd_image_0, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_image_recolor_opa(wd_image_0, 255, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

