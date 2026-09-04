/**
 * @file wf_analog_gen.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wf_analog_private_gen.h"
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

void wf_analog_constructor_hook(lv_obj_t * obj);
void wf_analog_destructor_hook(lv_obj_t * obj);
void wf_analog_event_hook(lv_event_t * e);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void wf_analog_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wf_analog_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wf_analog_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t wf_analog_class = {
    .base_class = &lv_image_class,
    .constructor_cb = wf_analog_constructor,
    .destructor_cb = wf_analog_destructor,
    .event_cb = wf_analog_event,
    .instance_size = sizeof(wf_analog_t),
    .editable = 1,
    .name = "wf_analog"
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * wf_analog_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&wf_analog_class, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void wf_analog_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    wf_analog_t * widget = (wf_analog_t *)obj;
    static lv_style_t style_main;
    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_main);

        lv_style_set_width(&style_main, LV_SIZE_CONTENT);
        lv_style_set_height(&style_main, LV_SIZE_CONTENT);

        style_inited = true;
    }

    lv_obj_t * the_root = NULL;
    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_add_style(obj, &style_main, 0);

        the_root = obj;
    }
    #endif
    wf_analog_constructor_hook(obj);

    LV_TRACE_OBJ_CREATE("finished");
}

static void wf_analog_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    wf_analog_destructor_hook(obj);
}

static void wf_analog_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    res = lv_obj_event_base(&wf_analog_class, e);
    if(res != LV_RESULT_OK) return;

    wf_analog_event_hook(e);
}

