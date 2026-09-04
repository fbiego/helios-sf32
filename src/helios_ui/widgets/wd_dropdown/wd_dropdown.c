/**
 * @file wd_dropdown.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_dropdown_private_gen.h"
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
static void value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void set_selected_item(wd_dropdown_t * widget, int32_t index);
static void on_item_deleted_cb(lv_event_t *e);
static void on_item_clicked_cb(lv_event_t *e);

static void on_main_clicked_cb(lv_event_t *e);
static void on_list_defocused_cb(lv_event_t *e);
char * wd_split_str(char ** src, char delimiter);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void wd_dropdown_constructor_hook(lv_obj_t * obj)
{
    wd_dropdown_t * widget = (wd_dropdown_t *)obj;

    lv_obj_add_event_cb(obj, on_main_clicked_cb, LV_EVENT_CLICKED, widget);
    lv_obj_add_event_cb(widget->dropdown_cont, on_list_defocused_cb, LV_EVENT_CLICKED, widget);
}

void wd_dropdown_destructor_hook(lv_obj_t * obj)
{
    wd_dropdown_t * widget = (wd_dropdown_t *)obj;
    // if (widget->options) {
    //     lv_free(widget->options);
    // }
}

void wd_dropdown_event_hook(lv_event_t * e)
{

}

void wd_dropdown_set_options(lv_obj_t * wd_dropdown, const char * options)
{
    wd_dropdown_t * widget = (wd_dropdown_t *)wd_dropdown;

    char * value_buf = lv_strdup(options);
    char * tmp = value_buf;

    const char * value = wd_split_str(&tmp, '\n');
    while(value) {
        lv_obj_t * item = hs_dropdown_item_create(widget->dropdown_list, value, value);
        lv_obj_add_event_cb(item, on_item_clicked_cb, LV_EVENT_CLICKED, widget);
        char * tag = lv_strdup(value);
        lv_obj_set_user_data(item, tag);
        lv_obj_add_event_cb(item, on_item_deleted_cb, LV_EVENT_DELETE, tag);

        value = wd_split_str(&tmp, '\n');
    }

    lv_free(value_buf);

    if (widget->bind_value) {
        widget->selected = lv_subject_get_int(widget->bind_value);
    }
    set_selected_item(widget, widget->selected);
}

void wd_dropdown_set_selected(lv_obj_t * wd_dropdown, int32_t selected)
{
    wd_dropdown_t * widget = (wd_dropdown_t *)wd_dropdown;
    widget->selected = selected;
}

void wd_dropdown_bind_value(lv_obj_t * wd_dropdown, lv_subject_t * bind_value)
{
    wd_dropdown_t * widget = (wd_dropdown_t *)wd_dropdown;
    widget->bind_value = bind_value;
    lv_subject_add_observer_obj(widget->bind_value, value_observer_cb, wd_dropdown, widget);

    if (widget->bind_value) {
        widget->selected = lv_subject_get_int(widget->bind_value);
    }
    set_selected_item(widget, widget->selected);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    wd_dropdown_t * widget = (wd_dropdown_t *)lv_observer_get_user_data(observer);
    int32_t index = lv_subject_get_int(subject);
    widget->selected = index;
    set_selected_item(widget, index);
}

static void set_selected_item(wd_dropdown_t * widget, int32_t index)
{
    lv_obj_t * obj = (lv_obj_t *)widget->dropdown_list;
    int32_t count = lv_obj_get_child_count(obj);
    for (int32_t i = 0; i < count; i++)
    {
        lv_obj_t *child = lv_obj_get_child(obj, i);
        lv_obj_set_state(child, LV_STATE_CHECKED, i == index);

        if (i == index) {
            lv_obj_t * label = lv_obj_get_child(child, 0);
            if (label) {
                lv_label_set_text(widget->main_label, lv_label_get_text(label));
                char * tag = lv_obj_get_user_data(child);
                if (tag) {
                    lv_label_set_translation_tag(widget->main_label, tag);
                }
            }
        }
    }
}

static void on_item_deleted_cb(lv_event_t *e)
{
    char *tag = lv_event_get_user_data(e);
    if (tag) {
        lv_free(tag);
        tag = NULL;
    }

}

static void on_item_clicked_cb(lv_event_t *e)
{
    lv_obj_t *clicked = lv_event_get_target(e);
    wd_dropdown_t * widget = (wd_dropdown_t *)lv_event_get_user_data(e);
    lv_obj_t * obj = (lv_obj_t *)widget->dropdown_list;
    for (int32_t i = 0; i < lv_obj_get_child_count(obj); i++)
    {
        lv_obj_t *child = lv_obj_get_child(obj, i);
        lv_obj_set_state(child, LV_STATE_CHECKED, child == clicked);
        if (child == clicked) {
            widget->selected = i;
            if (widget->bind_value) {
                lv_subject_set_int(widget->bind_value, widget->selected);
            }
            set_selected_item(widget, widget->selected);
        }
    }
    lv_obj_remove_state(widget->dropdown_cont, LV_STATE_CHECKED);
    lv_obj_add_flag(widget->dropdown_cont, LV_OBJ_FLAG_HIDDEN);
}

static void on_main_clicked_cb(lv_event_t *e)
{
    wd_dropdown_t * widget = (wd_dropdown_t *)lv_event_get_user_data(e);
    lv_obj_t *obj = (lv_obj_t *)widget;

    if (widget->bind_value) {
        widget->selected = lv_subject_get_int(widget->bind_value);
    }

    LV_LOG_USER("Selected item %d", widget->selected);

    lv_obj_t * selected = lv_obj_get_child(widget->dropdown_list, widget->selected);
    
    

    int32_t height = lv_obj_get_height(widget->dropdown_list);
    int32_t h_60 = lv_obj_get_height(lv_obj_get_screen(obj)) * 0.6;
    int32_t width = lv_obj_get_width(obj);

    lv_obj_set_width(widget->dropdown_list, width);
    if (height > h_60) lv_obj_set_height(widget->dropdown_list, h_60);
    lv_obj_add_state(widget->dropdown_cont, LV_STATE_CHECKED);
    lv_obj_set_parent(widget->dropdown_cont, lv_obj_get_screen(widget->dropdown_cont));
    lv_obj_move_to_index(widget->dropdown_cont, -1);

    if (selected) lv_obj_scroll_to_view(selected, LV_ANIM_ON);
    
    lv_obj_remove_flag(widget->dropdown_cont, LV_OBJ_FLAG_HIDDEN);


}

static void on_list_defocused_cb(lv_event_t *e)
{
    wd_dropdown_t * widget = (wd_dropdown_t *)lv_event_get_user_data(e);

    lv_obj_remove_state(widget->dropdown_cont, LV_STATE_CHECKED);
    lv_obj_add_flag(widget->dropdown_cont, LV_OBJ_FLAG_HIDDEN);
}

char * wd_split_str(char ** src, char delimiter)
{
    /*Skip multiple delimiters*/
    while(*src[0] == delimiter) {
        (*src)++;
    }

    if(*src[0] == '\0') return NULL;

    char * src_first = *src;
    char * src_next = *src;

    /*Find the delimiter*/
    while(*src_next != '\0') {
        if(*src_next == delimiter) {
            *src_next = '\0';       /*Close the string on the delimiter*/
            *src = src_next + 1;    /*Change the source continue after the found delimiter*/
            return src_first;
        }
        src_next++;
    }

    /*No delimiter found, return the string as it is*/
    *src = src_next;    /*Move the source point to the end*/

    return src_first;
}