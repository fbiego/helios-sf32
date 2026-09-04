/**
 * @file wf_analog.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wf_analog_private_gen.h"
#include "../../helios_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void subject_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void wf_analog_constructor_hook(lv_obj_t * obj)
{

}

void wf_analog_destructor_hook(lv_obj_t * obj)
{

}

void wf_analog_event_hook(lv_event_t * e)
{

}

void wf_analog_bind_rotation(lv_obj_t * wf_analog, lv_subject_t * bind_rotation)
{
    wf_analog_t * widget = (wf_analog_t *)wf_analog;
    widget->bind_rotation = bind_rotation;
    lv_subject_add_observer_obj(widget->bind_rotation, subject_observer_cb, wf_analog, widget);
}

void wf_analog_set_src(lv_obj_t * wf_analog, const void * src)
{
    wf_analog_t * widget = (wf_analog_t *)wf_analog;
    widget->src = src;
    lv_image_set_src(wf_analog, src);
}

void wf_analog_set_pivot_x(lv_obj_t * wf_analog, int32_t pivot_x)
{
    wf_analog_t * widget = (wf_analog_t *)wf_analog;
    widget->pivot_x = pivot_x;
    lv_obj_set_style_transform_pivot_x(wf_analog, widget->pivot_x, 0);
}

void wf_analog_set_pivot_y(lv_obj_t * wf_analog, int32_t pivot_y)
{
    wf_analog_t * widget = (wf_analog_t *)wf_analog;
    widget->pivot_y = pivot_y;
    lv_obj_set_style_transform_pivot_y(wf_analog, widget->pivot_y, 0);
}

void wf_analog_set_offset(lv_obj_t * wf_analog, int32_t offset)
{
    wf_analog_t * widget = (wf_analog_t *)wf_analog;
    widget->offset = offset;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void subject_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    wf_analog_t * widget = (wf_analog_t *)lv_observer_get_user_data(observer);
    lv_obj_t * obj = (lv_obj_t *)widget;
    lv_obj_set_style_transform_rotation(obj, lv_subject_get_int(subject) + widget->offset, 0);
}
