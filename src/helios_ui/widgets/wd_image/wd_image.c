/**
 * @file wd_image.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_image_private_gen.h"
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
static void scale_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void src_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void color_observer_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void wd_image_constructor_hook(lv_obj_t * obj)
{
    wd_image_t * widget = (wd_image_t *)obj;
    widget->scale_0 = 256;
    widget->scale_1 = 256;
    widget->scale_2 = 256;
    widget->size_0 = LV_SIZE_CONTENT;
    widget->size_1 = LV_SIZE_CONTENT;
    widget->size_2 = LV_SIZE_CONTENT;


}

void wd_image_destructor_hook(lv_obj_t * obj)
{

}

void wd_image_event_hook(lv_event_t * e)
{

}

void wd_image_set_src(lv_obj_t * wd_image, const void * src)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->src = src;
    lv_image_set_src(wd_image, widget->src);

}

void wd_image_set_scale_0(lv_obj_t * wd_image, int32_t scale_0)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->scale_0 = scale_0;
}

void wd_image_set_scale_1(lv_obj_t * wd_image, int32_t scale_1)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->scale_1 = scale_1;
}

void wd_image_set_scale_2(lv_obj_t * wd_image, int32_t scale_2)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->scale_2 = scale_2;
}

void wd_image_set_size_0(lv_obj_t * wd_image, int32_t size_0)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->size_0 = size_0;
}

void wd_image_set_size_1(lv_obj_t * wd_image, int32_t size_1)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->size_1 = size_1;
}

void wd_image_set_size_2(lv_obj_t * wd_image, int32_t size_2)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->size_2 = size_2;
}

void wd_image_set_x_0(lv_obj_t * wd_image, int32_t x_0)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->x_0 = x_0;
}

void wd_image_set_x_1(lv_obj_t * wd_image, int32_t x_1)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->x_1 = x_1;
}

void wd_image_set_x_2(lv_obj_t * wd_image, int32_t x_2)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->x_2 = x_2;
}

void wd_image_set_rotation(lv_obj_t * wd_image, int32_t rotation)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->rotation = rotation;
    lv_image_set_rotation(wd_image, widget->rotation);
}

void wd_image_bind_scale(lv_obj_t * wd_image, lv_subject_t * bind_scale)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->bind_scale = bind_scale;
    lv_subject_add_observer_obj(widget->bind_scale, scale_observer_cb, wd_image, widget);

}

void wd_image_bind_src(lv_obj_t * wd_image, lv_subject_t * bind_src)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->bind_src = bind_src;
    lv_subject_add_observer_obj(widget->bind_src, src_observer_cb, wd_image, widget);
}

void wd_image_bind_color(lv_obj_t * wd_image, lv_subject_t * bind_color)
{
    wd_image_t * widget = (wd_image_t *)wd_image;
    widget->bind_color = bind_color;
    lv_subject_add_observer_obj(widget->bind_color, color_observer_cb, wd_image, widget);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void scale_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    wd_image_t * widget = lv_observer_get_user_data(observer);
    lv_obj_t * obj = (lv_obj_t *)widget;
    int32_t index = lv_subject_get_int(subject);

    switch (index) {
        case 0: 
            lv_image_set_scale_x(obj, widget->scale_0);
            lv_image_set_scale_y(obj, widget->scale_0);
            lv_obj_set_size(obj, widget->size_0, widget->size_0);
            lv_obj_set_x(obj, widget->x_0);
            break;
        case 1: 
            lv_image_set_scale_x(obj, widget->scale_1);
            lv_image_set_scale_y(obj, widget->scale_1);
            lv_obj_set_size(obj, widget->size_1, widget->size_1);
            lv_obj_set_x(obj, widget->x_1);
            break;
        case 2: 
            lv_image_set_scale_x(obj, widget->scale_2);
            lv_image_set_scale_y(obj, widget->scale_2);
            lv_obj_set_size(obj, widget->size_2, widget->size_2);
            lv_obj_set_x(obj, widget->x_2);
            break;
        default:
            lv_image_set_scale_x(obj, 256);
            lv_image_set_scale_y(obj, 256);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_x(obj, 0);
    }
    lv_image_set_rotation(obj, widget->rotation);

}

static void src_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{

    if(subject->type != LV_SUBJECT_TYPE_POINTER) {
        return;
    }
    wd_image_t * widget = lv_observer_get_user_data(observer);
    lv_obj_t * obj = (lv_obj_t *)widget;
    const void *src = lv_subject_get_pointer(subject);

    lv_image_set_src(obj, src);
}

static void color_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_COLOR) {
        return;
    }
    wd_image_t * widget = lv_observer_get_user_data(observer);
    lv_obj_t * obj = (lv_obj_t *)widget;
    lv_color_t color = lv_subject_get_color(subject);
    
    lv_obj_set_style_image_recolor(obj, color, 0);
}
