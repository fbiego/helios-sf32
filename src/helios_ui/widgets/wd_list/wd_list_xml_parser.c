/**
 * @file wd_list_xml_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_list_gen.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "src/lvgl_private.h"
#else
    #include "lvgl/src/lvgl_private.h"
#endif

#if defined(LV_USE_XML) && LV_USE_XML

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * wd_list_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = wd_list_create(lv_xml_state_get_parent(state));

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wd_list");
        return NULL;
    }

    return item;
}

void wd_list_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("bind_mode", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                wd_list_bind_mode(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in wd_list bind_mode", value);
            }
        }

        if(lv_streq("bind_circular", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                wd_list_bind_circular(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in wd_list bind_circular", value);
            }
        }

        if(lv_streq("bind_screen", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                wd_list_bind_screen(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in wd_list bind_screen", value);
            }
        }
        if(lv_streq("center_lock", name)) {
            wd_list_set_center_lock(item, lv_xml_to_bool(value));
        }
    }
}

void * wd_list_container_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wd_list-container");
        return NULL;
    }

    return wd_list_get_container(item);
}

void wd_list_container_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * parent_item = lv_xml_state_get_parent(state);
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];
    }
}

void * wd_list_title_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wd_list-title");
        return NULL;
    }

    return wd_list_get_title(item);
}

void wd_list_title_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * parent_item = lv_xml_state_get_parent(state);
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];
    }
}

void wd_list_register(void)
{
    lv_xml_register_widget("wd_list", wd_list_xml_create, wd_list_xml_apply);
    lv_xml_register_widget("wd_list-container", wd_list_container_xml_create, wd_list_container_xml_apply);
    lv_xml_register_widget("wd_list-title", wd_list_title_xml_create, wd_list_title_xml_apply);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */