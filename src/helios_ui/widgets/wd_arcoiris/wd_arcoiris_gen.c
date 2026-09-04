/**
 * @file wd_arcoiris_gen.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_arcoiris_private_gen.h"
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

void wd_arcoiris_constructor_hook(lv_obj_t * obj);
void wd_arcoiris_destructor_hook(lv_obj_t * obj);
void wd_arcoiris_event_hook(lv_event_t * e);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void wd_arcoiris_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wd_arcoiris_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wd_arcoiris_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t wd_arcoiris_class = {
    .base_class = &lv_obj_class,
    .constructor_cb = wd_arcoiris_constructor,
    .destructor_cb = wd_arcoiris_destructor,
    .event_cb = wd_arcoiris_event,
    .instance_size = sizeof(wd_arcoiris_t),
    .editable = 1,
    .name = "wd_arcoiris"
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * wd_arcoiris_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&wd_arcoiris_class, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void wd_arcoiris_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    wd_arcoiris_t * widget = (wd_arcoiris_t *)obj;
    static lv_style_t style_main;
    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_main);

        lv_style_set_width(&style_main, lv_pct(100));
        lv_style_set_height(&style_main, lv_pct(100));
        lv_style_set_bg_color(&style_main, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_main, 0);

        style_inited = true;
    }

    lv_obj_t * the_root = NULL;
    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_add_style(obj, &style_main, 0);

        the_root = obj;
    }
    #endif
    wd_arcoiris_constructor_hook(obj);

    LV_TRACE_OBJ_CREATE("finished");
}

static void wd_arcoiris_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    wd_arcoiris_destructor_hook(obj);
}

static void wd_arcoiris_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    res = lv_obj_event_base(&wd_arcoiris_class, e);
    if(res != LV_RESULT_OK) return;

    wd_arcoiris_event_hook(e);
}

