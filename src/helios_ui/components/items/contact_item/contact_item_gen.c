/**
 * @file contact_item_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "contact_item_gen.h"
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

lv_obj_t * contact_item_create(lv_obj_t * parent, const char * name, const char * number)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, LV_SIZE_CONTENT);
        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_base, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_track_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_base, LV_FLEX_ALIGN_CENTER);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x191919));
        lv_style_set_radius(&style_base, 30);
        lv_style_set_pad_ver(&style_base, 10);
        lv_style_set_pad_hor(&style_base, 20);
        lv_style_set_pad_column(&style_base, 10);
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_pad_ver(&style_base_360, 6);
        lv_style_set_pad_hor(&style_base_360, 12);
        lv_style_set_radius(&style_base_360, 24);
        lv_style_set_pad_ver(&style_base_240, 4);
        lv_style_set_pad_hor(&style_base_240, 8);
        lv_style_set_radius(&style_base_240, 18);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "contact_item_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_column_0 = hs_column_create(lv_obj_0);
        lv_obj_set_flex_grow(hs_column_0, 1);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(hs_column_0);
        lv_label_set_text(hs_text_normal_0, name);

        lv_obj_t * hs_text_normal_1 = hs_text_normal_create(hs_column_0);
        lv_label_set_text(hs_text_normal_1, number);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

