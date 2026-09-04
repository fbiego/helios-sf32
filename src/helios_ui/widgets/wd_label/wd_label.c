/**
 * @file wd_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_label_private_gen.h"
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
static void text_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void tag_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void wd_label_constructor_hook(lv_obj_t * obj)
{

}

void wd_label_destructor_hook(lv_obj_t * obj)
{

}

void wd_label_event_hook(lv_event_t * e)
{

}

void wd_label_set_text(lv_obj_t * wd_label, const char * text)
{
    lv_label_set_text(wd_label, text);
}

void wd_label_set_tag(lv_obj_t * wd_label, const char * tag)
{
    lv_label_set_translation_tag(wd_label, tag);
}

void wd_label_bind_text(lv_obj_t * wd_label, lv_subject_t * bind_text)
{
    wd_label_t * widget = (wd_label_t *)wd_label;
    widget->bind_text = bind_text;
    lv_subject_add_observer_obj(widget->bind_text, text_observer_cb, wd_label, widget);

    const char * text = lv_subject_get_string(widget->bind_text);
    lv_label_set_text(wd_label, text);
}

void wd_label_bind_tag(lv_obj_t * wd_label, lv_subject_t * bind_tag)
{
    wd_label_t * widget = (wd_label_t *)wd_label;
    widget->bind_tag = bind_tag;
    lv_subject_add_observer_obj(widget->bind_tag, tag_observer_cb, wd_label, widget);

    const char * tag = lv_subject_get_string(widget->bind_tag);
    lv_label_set_translation_tag(wd_label, tag);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void text_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const char * text = lv_subject_get_string(subject);
    wd_label_t * widget = (wd_label_t *)lv_observer_get_user_data(observer);
    lv_obj_t * label = (lv_obj_t *)widget;
    lv_label_set_text(label, text);
}

static void tag_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const char * tag = lv_subject_get_string(subject);
    wd_label_t * widget = (wd_label_t *)lv_observer_get_user_data(observer);
    lv_obj_t * label = (lv_obj_t *)widget;
    lv_label_set_translation_tag(label, tag);
}
