/**
 * @file wf_item_gen.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wf_item_private_gen.h"
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

void wf_item_constructor_hook(lv_obj_t * obj);
void wf_item_destructor_hook(lv_obj_t * obj);
void wf_item_event_hook(lv_event_t * e);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void wf_item_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wf_item_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wf_item_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t wf_item_class = {
    .base_class = &lv_image_class,
    .constructor_cb = wf_item_constructor,
    .destructor_cb = wf_item_destructor,
    .event_cb = wf_item_event,
    .instance_size = sizeof(wf_item_t),
    .editable = 1,
    .name = "wf_item"
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * wf_item_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&wf_item_class, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void wf_item_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    wf_item_t * widget = (wf_item_t *)obj;
    static lv_style_t style_base;
    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_base);

        lv_style_set_width(&style_base, LV_SIZE_CONTENT);
        lv_style_set_height(&style_base, LV_SIZE_CONTENT);

        style_inited = true;
    }

    lv_obj_t * the_root = NULL;
    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_add_style(obj, &style_base, 0);

        the_root = obj;
    }
    #endif
    wf_item_constructor_hook(obj);

    LV_TRACE_OBJ_CREATE("finished");
}

static void wf_item_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    wf_item_destructor_hook(obj);
}

static void wf_item_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    res = lv_obj_event_base(&wf_item_class, e);
    if(res != LV_RESULT_OK) return;

    wf_item_event_hook(e);
}

