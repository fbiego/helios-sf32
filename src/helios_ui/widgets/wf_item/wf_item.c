/**
 * @file wf_item.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wf_item_private_gen.h"
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
static void subject_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void set_item_digit(wf_item_t * widget, int32_t value);
static uint8_t get_digit(int32_t value, uint8_t p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void wf_item_constructor_hook(lv_obj_t * obj)
{
    wf_item_t * widget = (wf_item_t *)obj;
    widget->place_value = 0;
    widget->image_array_count = 0;
}

void wf_item_destructor_hook(lv_obj_t * obj)
{

}

void wf_item_event_hook(lv_event_t * e)
{

}

void wf_item_set_image_array(lv_obj_t * wf_item, const void * image_array[], uint32_t image_array_count)
{
    wf_item_t * widget = (wf_item_t *)wf_item;
    widget->image_array = image_array;
    widget->image_array_count = image_array_count;
}

void wf_item_bind_subject(lv_obj_t * wf_item, lv_subject_t * bind_subject)
{
    wf_item_t * widget = (wf_item_t *)wf_item;
    widget->bind_subject = bind_subject;

    lv_subject_add_observer_obj(widget->bind_subject, subject_value_observer_cb, wf_item, widget);

    int32_t value = lv_subject_get_int(widget->bind_subject);
    set_item_digit(widget, value);
}

void wf_item_set_place_value(lv_obj_t * wf_item, int32_t place_value)
{
    wf_item_t * widget = (wf_item_t *)wf_item;
    widget->place_value = place_value;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void subject_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    wf_item_t * widget = (wf_item_t *)lv_observer_get_user_data(observer);
    int32_t value = lv_subject_get_int(subject);
    set_item_digit(widget, value);
}

static void set_item_digit(wf_item_t * widget, int32_t value)
{
    lv_obj_t * obj = (lv_obj_t *)widget;

    if (value < 0) {
        lv_image_set_src(obj, NULL);
        return;
    }
    uint8_t v = get_digit(value, widget->place_value);

    if (v >= widget->image_array_count) v = 0;

    if (widget->image_array_count <= 0) return;

    lv_image_set_src(obj, widget->image_array[v]);
}

static uint8_t get_digit(int32_t value, uint8_t p)
{
    uint32_t divisor = 1;

    for (uint8_t i = 0; i < p; i++) {
        divisor *= 10;
    }

    uint32_t magnitude = value < 0
        ? (uint32_t)(-(int64_t)value)
        : (uint32_t)value;

    return (magnitude / divisor) % 10;
}