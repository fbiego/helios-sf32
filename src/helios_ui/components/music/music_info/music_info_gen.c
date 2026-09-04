/**
 * @file music_info_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "music_info_gen.h"
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

lv_obj_t * music_info_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_music;
    static lv_style_t style_music_360;
    static lv_style_t style_music_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_music);
        lv_style_init(&style_music_360);
        lv_style_init(&style_music_240);

        lv_style_set_width(&style_music, 360);
        lv_style_set_height(&style_music, 110);
        lv_style_set_pad_all(&style_music, 15);
        lv_style_set_pad_bottom(&style_music, 8);
        lv_style_set_radius(&style_music, 40);
        lv_style_set_bg_opa(&style_music, 100);
        lv_style_set_layout(&style_music, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_music, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_main_place(&style_music, LV_FLEX_ALIGN_SPACE_BETWEEN);
        lv_style_set_width(&style_music_360, 290);
        lv_style_set_height(&style_music_360, 90);
        lv_style_set_radius(&style_music_360, 30);
        lv_style_set_pad_all(&style_music_360, 13);
        lv_style_set_pad_bottom(&style_music_360, 8);
        lv_style_set_width(&style_music_240, 200);
        lv_style_set_height(&style_music_240, 60);
        lv_style_set_radius(&style_music_240, 20);
        lv_style_set_pad_all(&style_music_240, 10);
        lv_style_set_pad_bottom(&style_music_240, 5);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * wd_obj_0 = wd_obj_create(parent);
        lv_obj_set_name_static(wd_obj_0, "music_info_#");
        lv_obj_set_flag(wd_obj_0, LV_OBJ_FLAG_EVENT_BUBBLE, true);
        wd_obj_bind_bg_color(wd_obj_0, &sb_music_album_color);

        lv_obj_add_style(wd_obj_0, &style_music, 0);
        lv_obj_bind_style(wd_obj_0, &style_music_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_obj_0, &style_music_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(wd_obj_0);
        lv_label_bind_text(hs_text_normal_0, &sb_music_track, NULL);
        lv_obj_set_width(hs_text_normal_0, lv_pct(100));
        lv_obj_set_style_text_align(hs_text_normal_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(hs_text_normal_0, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);

        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(wd_obj_0);
        lv_label_bind_text(hs_text_normal_1, &sb_music_artist, NULL);
        lv_obj_set_width(hs_text_normal_1, lv_pct(100));
        lv_obj_set_style_text_align(hs_text_normal_1, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(hs_text_normal_1, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);

        the_root = wd_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

