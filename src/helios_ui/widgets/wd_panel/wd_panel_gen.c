/**
 * @file wd_panel_gen.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_panel_private_gen.h"
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

void wd_panel_constructor_hook(lv_obj_t * obj);
void wd_panel_destructor_hook(lv_obj_t * obj);
void wd_panel_event_hook(lv_event_t * e);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void wd_panel_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wd_panel_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wd_panel_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t wd_panel_class = {
    .base_class = &lv_obj_class,
    .constructor_cb = wd_panel_constructor,
    .destructor_cb = wd_panel_destructor,
    .event_cb = wd_panel_event,
    .instance_size = sizeof(wd_panel_t),
    .editable = 1,
    .name = "wd_panel"
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * wd_panel_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&wd_panel_class, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void wd_panel_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    wd_panel_t * widget = (wd_panel_t *)obj;
    static lv_style_t style_base;
    static lv_style_t style_panel_dot;
    static lv_style_t style_panel_dot_360;
    static lv_style_t style_panel_dot_240;
    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);
        lv_style_init(&style_panel_dot);
        lv_style_init(&style_panel_dot_360);
        lv_style_init(&style_panel_dot_240);

        lv_style_set_width(&style_base, lv_pct(100));
        lv_style_set_height(&style_base, lv_pct(100));
        lv_style_set_bg_opa(&style_base, 0);
        lv_style_set_width(&style_panel_dot, lv_pct(100));
        lv_style_set_height(&style_panel_dot, LV_SIZE_CONTENT);
        lv_style_set_align(&style_panel_dot, LV_ALIGN_BOTTOM_MID);
        lv_style_set_layout(&style_panel_dot, LV_LAYOUT_FLEX);
        lv_style_set_flex_main_place(&style_panel_dot, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_bottom(&style_panel_dot, 12);
        lv_style_set_pad_column(&style_panel_dot, 9);
        lv_style_set_pad_bottom(&style_panel_dot_360, 9);
        lv_style_set_pad_column(&style_panel_dot_360, 6);
        lv_style_set_pad_bottom(&style_panel_dot_240, 7);
        lv_style_set_pad_column(&style_panel_dot_240, 3);

        style_inited = true;
    }

    lv_obj_t * the_root = NULL;
    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_add_style(obj, &style_base, 0);
        lv_obj_t * container = lv_obj_create(obj);
        lv_obj_set_name(container, "container");
        widget->container = container;
        lv_obj_remove_style_all(container);
        lv_obj_add_style(container, &style_base, 0);

        lv_obj_t * dots = lv_obj_create(obj);
        lv_obj_set_name(dots, "dots");
        widget->dots = dots;
        lv_obj_remove_style_all(dots);
        lv_obj_add_style(dots, &style_panel_dot, 0);
        lv_obj_bind_style(dots, &style_panel_dot_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(dots, &style_panel_dot_240, 0, &sb_screen_size, 2);

        the_root = obj;
    }
    #endif
    wd_panel_constructor_hook(obj);

    LV_TRACE_OBJ_CREATE("finished");
}

static void wd_panel_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    wd_panel_destructor_hook(obj);
}

static void wd_panel_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    res = lv_obj_event_base(&wd_panel_class, e);
    if(res != LV_RESULT_OK) return;

    wd_panel_event_hook(e);
}

