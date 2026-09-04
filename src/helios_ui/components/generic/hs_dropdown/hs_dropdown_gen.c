/**
 * @file hs_dropdown_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "hs_dropdown_gen.h"
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

lv_obj_t * hs_dropdown_create(lv_obj_t * parent, const char * label, const char * tag, lv_subject_t * bind_value, const char * options)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;
    static lv_style_t style_dropdown;
    static lv_style_t style_dropdown_360;
    static lv_style_t style_dropdown_240;
    static lv_style_t style_dropdown_list;
    static lv_style_t style_dropdown_list_360;
    static lv_style_t style_dropdown_list_240;
    static lv_style_t style_dropdown_selected;
    static lv_style_t style_scrollbar;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);
        lv_style_init(&style_dropdown);
        lv_style_init(&style_dropdown_360);
        lv_style_init(&style_dropdown_240);
        lv_style_init(&style_dropdown_list);
        lv_style_init(&style_dropdown_list_360);
        lv_style_init(&style_dropdown_list_240);
        lv_style_init(&style_dropdown_selected);
        lv_style_init(&style_scrollbar);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, LV_SIZE_CONTENT);
        lv_style_set_layout(&style_base, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_base, LV_FLEX_FLOW_COLUMN);
        lv_style_set_text_color(&style_base, lv_color_hex3(0xfff));
        lv_style_set_radius(&style_base, 10);
        lv_style_set_pad_ver(&style_base, 0);
        lv_style_set_pad_row(&style_base, 10);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x171717));
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_pad_row(&style_base_360, 4);
        lv_style_set_pad_row(&style_base_240, 2);
        lv_style_set_width(&style_dropdown, lv_pct(100));
        lv_style_set_bg_color(&style_dropdown, lv_color_hex(0x3a3a3a));
        lv_style_set_height(&style_dropdown, 46);
        lv_style_set_text_color(&style_dropdown, lv_color_hex3(0xfff));
        lv_style_set_border_width(&style_dropdown, 0);
        lv_style_set_radius(&style_dropdown, 6);
        lv_style_set_height(&style_dropdown_360, 36);
        lv_style_set_radius(&style_dropdown_360, 4);
        lv_style_set_height(&style_dropdown_240, 24);
        lv_style_set_radius(&style_dropdown_240, 2);
        lv_style_set_bg_color(&style_dropdown_list, lv_color_hex(0x606060));
        lv_style_set_text_color(&style_dropdown_list, lv_color_hex3(0xfff));
        lv_style_set_border_width(&style_dropdown_list, 0);
        lv_style_set_pad_ver(&style_dropdown_list, 50);
        lv_style_set_pad_row(&style_dropdown_list, 10);
        lv_style_set_pad_ver(&style_dropdown_list_360, 40);
        lv_style_set_pad_ver(&style_dropdown_list_240, 30);
        lv_style_set_bg_color(&style_dropdown_selected, lv_color_hex3(0xfff));
        lv_style_set_text_color(&style_dropdown_selected, lv_color_hex3(0x000));
        lv_style_set_bg_color(&style_scrollbar, lv_color_hex3(0xfff));
        lv_style_set_pad_all(&style_scrollbar, 0);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "hs_dropdown_#");

        lv_obj_remove_style_all(lv_obj_0);
        lv_obj_add_style(lv_obj_0, &style_base, 0);
        lv_obj_bind_style(lv_obj_0, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(lv_obj_0, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_t * hs_text_normal_0 = hs_text_normal_create(lv_obj_0);
        lv_obj_set_width(hs_text_normal_0, lv_pct(100));
        lv_label_set_text(hs_text_normal_0, label);
        lv_label_set_translation_tag(hs_text_normal_0, tag);

        lv_obj_t * wd_dropdown_0 = wd_dropdown_create(lv_obj_0);
        wd_dropdown_bind_value(wd_dropdown_0, bind_value);
        wd_dropdown_set_options(wd_dropdown_0, options);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

