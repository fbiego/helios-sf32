/**
 * @file wd_label_gen.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_label_private_gen.h"
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

void wd_label_constructor_hook(lv_obj_t * obj);
void wd_label_destructor_hook(lv_obj_t * obj);
void wd_label_event_hook(lv_event_t * e);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void wd_label_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wd_label_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void wd_label_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t wd_label_class = {
    .base_class = &lv_label_class,
    .constructor_cb = wd_label_constructor,
    .destructor_cb = wd_label_destructor,
    .event_cb = wd_label_event,
    .instance_size = sizeof(wd_label_t),
    .editable = 1,
    .name = "wd_label"
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * wd_label_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&wd_label_class, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void wd_label_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    wd_label_t * widget = (wd_label_t *)obj;

    static bool style_inited = false;

    if (!style_inited) {

        style_inited = true;
    }

    lv_obj_t * the_root = NULL;
    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {


        the_root = obj;
    }
    #endif
    wd_label_constructor_hook(obj);

    LV_TRACE_OBJ_CREATE("finished");
}

static void wd_label_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    wd_label_destructor_hook(obj);
}

static void wd_label_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /* Call the ancestor's event handler */
    res = lv_obj_event_base(&wd_label_class, e);
    if(res != LV_RESULT_OK) return;

    wd_label_event_hook(e);
}

