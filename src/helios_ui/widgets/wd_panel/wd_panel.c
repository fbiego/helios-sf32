/**
 * @file wd_panel.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_panel_private_gen.h"
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
static void create_dots(wd_panel_t * widget);

static void on_scroll_end_cb(lv_event_t *e);
static void on_child_changed_cb(lv_event_t *e);

static void highlight_dot(wd_panel_t * widget, int32_t index);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void wd_panel_constructor_hook(lv_obj_t * obj)
{
    wd_panel_t * widget = (wd_panel_t *)obj;

    lv_obj_add_event_cb(widget->container, on_scroll_end_cb, LV_EVENT_SCROLL_END, widget);
    lv_obj_add_event_cb(widget->container, on_child_changed_cb, LV_EVENT_CHILD_CHANGED, widget);
}

void wd_panel_destructor_hook(lv_obj_t * obj)
{

}

void wd_panel_event_hook(lv_event_t * e)
{

}

lv_obj_t * wd_panel_get_container(lv_obj_t * wd_panel)
{
    wd_panel_t * widget = (wd_panel_t *)wd_panel;
    return widget->container;
}

lv_obj_t * wd_panel_get_dot(lv_obj_t * wd_panel)
{
    wd_panel_t * widget = (wd_panel_t *)wd_panel;
    return widget->dots;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void create_dots(wd_panel_t * widget)
{
    lv_obj_clean(widget->dots);

    int32_t count = lv_obj_get_child_count(widget->container);

    for (int32_t i = 0; i < count; i++) {
        panel_dot_item_create(widget->dots);
    }
}

static void on_scroll_end_cb(lv_event_t *e)
{
    wd_panel_t * widget = (wd_panel_t *)lv_event_get_user_data(e);
    
    int32_t count = lv_obj_get_child_count(widget->container);

    if (count < 1) return;

    int32_t c_w = lv_obj_get_width(lv_obj_get_child(widget->container, 0));

    if (c_w < 1) return;

    int32_t pos = lv_obj_get_scroll_x(widget->container);

    /* Scroll snapped */  
    void *param = lv_event_get_param(e);

    int32_t index = (pos + (c_w / 2)) / c_w;
    index = LV_CLAMP(0, index, count - 1);
    if (param == NULL){
        /* Scroll ended, snap position was adjusted */
        highlight_dot(widget, index);
    } else {
        /* User scroll ended */
        highlight_dot(widget, index); // highlight immediately
        if (pos % c_w == 0) {
            /* Scroll ended at exact item snap position */
            // highlight_dot(widget, index);
        }
    }

}

static void on_child_changed_cb(lv_event_t *e)
{
    wd_panel_t * widget = (wd_panel_t *)lv_event_get_user_data(e);

    create_dots(widget);
    lv_obj_send_event(widget->container, LV_EVENT_SCROLL_END, widget);
    
}

static void highlight_dot(wd_panel_t * widget, int32_t index)
{
    int32_t count = lv_obj_get_child_count(widget->dots);

    for (int32_t i = 0; i < count; i++) {
        lv_obj_t *dot = lv_obj_get_child(widget->dots, i);
        lv_obj_set_state(dot, LV_STATE_CHECKED, i == index);
    }
}

