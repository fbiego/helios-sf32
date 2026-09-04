/**
 * @file control_music_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "control_music_gen.h"
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

lv_obj_t * control_music_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_music;
    static lv_style_t style_music_360;
    static lv_style_t style_music_240;
    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_music);
        lv_style_init(&style_music_360);
        lv_style_init(&style_music_240);
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);

        lv_style_set_width(&style_music, 360);
        lv_style_set_height(&style_music, 110);
        lv_style_set_radius(&style_music, 55);
        lv_style_set_width(&style_music_360, 290);
        lv_style_set_height(&style_music_360, 90);
        lv_style_set_width(&style_music_240, 200);
        lv_style_set_height(&style_music_240, 60);
        lv_style_set_width(&style_cont, 320);
        lv_style_set_height(&style_cont, 60);
        lv_style_set_radius(&style_cont, 30);
        lv_style_set_bg_color(&style_cont, lv_color_hex(0x838383));
        lv_style_set_bg_opa(&style_cont, 100);
        lv_style_set_align(&style_cont, LV_ALIGN_CENTER);
        lv_style_set_width(&style_cont_360, 250);
        lv_style_set_height(&style_cont_360, 48);
        lv_style_set_width(&style_cont_240, 180);
        lv_style_set_height(&style_cont_240, 35);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "control_music_#");
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_EVENT_BUBBLE, true);

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_music, 0);
        lv_obj_bind_style(lv_obj_0, &style_music_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_music_240, 0, &sb_screen_size, 2);
        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_remove_style_all(lv_obj_1);
        lv_obj_add_style(lv_obj_1, &style_cont, 0);
        lv_obj_bind_style(lv_obj_1, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_1, &style_cont_240, 0, &sb_screen_size, 2);

        lv_obj_t * control_button_0 = control_button_create(lv_obj_0, icon_back);
        lv_obj_set_align(control_button_0, LV_ALIGN_LEFT_MID);
        lv_obj_add_event_cb(control_button_0, on_music_control_cb, LV_EVENT_CLICKED, "previous");

        lv_obj_t * control_button_1 = control_button_create(lv_obj_0, icon_play);
        lv_obj_set_align(control_button_1, LV_ALIGN_CENTER);
        lv_obj_add_event_cb(control_button_1, on_music_control_cb, LV_EVENT_CLICKED, "toggle");

        lv_obj_t * control_button_2 = control_button_create(lv_obj_0, icon_next);
        lv_obj_set_align(control_button_2, LV_ALIGN_RIGHT_MID);
        lv_obj_add_event_cb(control_button_2, on_music_control_cb, LV_EVENT_CLICKED, "next");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

