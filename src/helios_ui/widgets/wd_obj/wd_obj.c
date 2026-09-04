/**
 * @file wd_obj.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_obj_private_gen.h"
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
static void bg_color_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void wd_obj_constructor_hook(lv_obj_t * obj)
{

}

void wd_obj_destructor_hook(lv_obj_t * obj)
{

}

void wd_obj_event_hook(lv_event_t * e)
{

}

void wd_obj_bind_bg_color(lv_obj_t * wd_obj, lv_subject_t * bind_bg_color)
{
    wd_obj_t * widget = (wd_obj_t *)wd_obj;
    widget->bind_bg_color = bind_bg_color;
    lv_subject_add_observer_obj(widget->bind_bg_color, bg_color_observer_cb, wd_obj, widget);

    if(bind_bg_color->type != LV_SUBJECT_TYPE_COLOR) {
        return;
    }
    lv_color_t color = lv_subject_get_color(bind_bg_color);
    lv_obj_set_style_bg_color(wd_obj, color, 0);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void bg_color_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_COLOR) {
        return;
    }
    wd_obj_t * widget = lv_observer_get_user_data(observer);
    lv_obj_t * obj = (lv_obj_t *)widget;
    lv_color_t color = lv_subject_get_color(subject);
    
    lv_obj_set_style_bg_color(obj, color, 0);
}
