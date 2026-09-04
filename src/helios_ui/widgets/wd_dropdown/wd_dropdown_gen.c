/**
 * @file wd_dropdown_gen.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_dropdown_private_gen.h"
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif
#include "../../helios_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

void wd_dropdown_constructor_hook(lv_obj_t * obj);
void wd_dropdown_destructor_hook(lv_obj_t * obj);
void wd_dropdown_event_hook(lv_event_t * e);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void wd_dropdown_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wd_dropdown_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wd_dropdown_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t wd_dropdown_class = {
    .base_class = &lv_obj_class,
    .constructor_cb = wd_dropdown_constructor,
    .destructor_cb = wd_dropdown_destructor,
    .event_cb = wd_dropdown_event,
    .instance_size = sizeof(wd_dropdown_t),
    .editable = 1,
    .name = "wd_dropdown"
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * wd_dropdown_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&wd_dropdown_class, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void wd_dropdown_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    wd_dropdown_t * widget = (wd_dropdown_t *)obj;
    static lv_style_t style_base;
    static lv_style_t style_base_360;
    static lv_style_t style_base_240;
    static lv_style_t style_pressed;
    static lv_style_t style_text;
    static lv_style_t style_text_360;
    static lv_style_t style_text_240;
    static lv_style_t style_cont;
    static lv_style_t style_list;
    static lv_style_t style_list_360;
    static lv_style_t style_list_240;
    static lv_style_t style_scrollbar;
    static lv_style_t style_scrollbar_360;
    static lv_style_t style_scrollbar_240;
    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_base_360);
        lv_style_init(&style_base_240);
        lv_style_init(&style_pressed);
        lv_style_init(&style_text);
        lv_style_init(&style_text_360);
        lv_style_init(&style_text_240);
        lv_style_init(&style_cont);
        lv_style_init(&style_list);
        lv_style_init(&style_list_360);
        lv_style_init(&style_list_240);
        lv_style_init(&style_scrollbar);
        lv_style_init(&style_scrollbar_360);
        lv_style_init(&style_scrollbar_240);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_bg_color(&style_base, lv_color_hex(0x3a3a3a));
        lv_style_set_bg_opa(&style_base, 255);
        lv_style_set_height(&style_base, 46);
        lv_style_set_text_color(&style_base, lv_color_hex(0xffffff));
        lv_style_set_border_width(&style_base, 0);
        lv_style_set_radius(&style_base, 6);
        lv_style_set_height(&style_base_360, 36);
        lv_style_set_radius(&style_base_360, 4);
        lv_style_set_height(&style_base_240, 24);
        lv_style_set_radius(&style_base_240, 2);
        lv_style_set_bg_color(&style_pressed, lv_color_hex(0xa2a2a2));
        lv_style_set_bg_opa(&style_pressed, 255);
        lv_style_set_text_color(&style_pressed, lv_color_hex3(0x000));
        lv_style_set_pad_top(&style_text, 5);
        lv_style_set_pad_hor(&style_text, 10);
        lv_style_set_pad_top(&style_text_360, 2);
        lv_style_set_pad_hor(&style_text_360, 8);
        lv_style_set_pad_top(&style_text_240, 1);
        lv_style_set_pad_hor(&style_text_240, 5);
        lv_style_set_width(&style_cont, lv_pct(100));
        lv_style_set_height(&style_cont, lv_pct(100));
        lv_style_set_bg_color(&style_cont, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_cont, 220);
        lv_style_set_width(&style_list, lv_pct(100));
        lv_style_set_height(&style_list, LV_SIZE_CONTENT);
        lv_style_set_bg_color(&style_list, lv_color_hex(0x555555));
        lv_style_set_bg_opa(&style_list, 255);
        lv_style_set_layout(&style_list, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_list, LV_FLEX_FLOW_COLUMN);
        lv_style_set_pad_row(&style_list, 3);
        lv_style_set_pad_all(&style_list, 6);
        lv_style_set_radius(&style_list, 6);
        lv_style_set_align(&style_list, LV_ALIGN_CENTER);
        lv_style_set_pad_row(&style_list_360, 2);
        lv_style_set_pad_all(&style_list_360, 5);
        lv_style_set_radius(&style_list_360, 4);
        lv_style_set_pad_row(&style_list_240, 1);
        lv_style_set_pad_all(&style_list_240, 2);
        lv_style_set_radius(&style_list_240, 2);
        lv_style_set_width(&style_scrollbar, 2);
        lv_style_set_bg_opa(&style_scrollbar, 255);
        lv_style_set_bg_color(&style_scrollbar, lv_color_hex(0xdddddd));
        lv_style_set_pad_top(&style_scrollbar, 10);
        lv_style_set_pad_bottom(&style_scrollbar, 10);
        lv_style_set_width(&style_scrollbar_360, 2);
        lv_style_set_width(&style_scrollbar_240, 1);

        style_inited = true;
    }

    lv_obj_t * the_root = NULL;
    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_add_style(obj, &style_base, 0);
        lv_obj_bind_style(obj, &style_base_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(obj, &style_base_240, 0, &sb_screen_size, 2);
        lv_obj_add_style(obj, &style_pressed, LV_STATE_PRESSED);
        lv_obj_t * main_label = hs_text_small_create(obj);
        lv_obj_set_name(main_label, "main_label");
        lv_obj_set_width(main_label, lv_pct(100));
        lv_obj_set_align(main_label, LV_ALIGN_CENTER);
        widget->main_label = main_label;
        lv_obj_add_style(main_label, &style_text, 0);
        lv_obj_bind_style(main_label, &style_text_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(main_label, &style_text_240, 0, &sb_screen_size, 2);

        lv_obj_t * dropdown_cont = lv_obj_create(obj);
        lv_obj_set_name(dropdown_cont, "dropdown_cont");
        lv_obj_set_flag(dropdown_cont, LV_OBJ_FLAG_HIDDEN, true);
        widget->dropdown_cont = dropdown_cont;
        lv_obj_remove_style_all(dropdown_cont);
        lv_obj_add_style(dropdown_cont, &style_cont, 0);
        lv_obj_t * dropdown_list = lv_obj_create(dropdown_cont);
        lv_obj_set_name(dropdown_list, "dropdown_list");
        lv_obj_set_flag(dropdown_list, LV_OBJ_FLAG_SCROLL_ON_FOCUS, false);
        widget->dropdown_list = dropdown_list;
        lv_obj_remove_style_all(dropdown_list);
        lv_obj_add_style(dropdown_list, &style_list, 0);
        lv_obj_bind_style(dropdown_list, &style_list_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(dropdown_list, &style_list_240, 0, &sb_screen_size, 2);
        lv_obj_add_style(dropdown_list, &style_scrollbar, LV_PART_SCROLLBAR);
        lv_obj_bind_style(dropdown_list, &style_scrollbar_360, LV_PART_SCROLLBAR, &sb_screen_size, 1);
        lv_obj_bind_style(dropdown_list, &style_scrollbar_240, LV_PART_SCROLLBAR, &sb_screen_size, 2);

        the_root = obj;
    }
    #endif
    wd_dropdown_constructor_hook(obj);

    LV_TRACE_OBJ_CREATE("finished");
}

static void wd_dropdown_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    wd_dropdown_destructor_hook(obj);
}

static void wd_dropdown_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    res = lv_obj_event_base(&wd_dropdown_class, e);
    if(res != LV_RESULT_OK) return;

    wd_dropdown_event_hook(e);
}

