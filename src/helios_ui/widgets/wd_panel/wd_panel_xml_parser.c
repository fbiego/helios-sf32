/**
 * @file wd_panel_xml_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_panel_gen.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
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

void * wd_panel_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = wd_panel_create(lv_xml_state_get_parent(state));

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wd_panel");
        return NULL;
    }

    return item;
}

void wd_panel_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];
    }
}

void * wd_panel_container_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wd_panel-container");
        return NULL;
    }

    return wd_panel_get_container(item);
}

void wd_panel_container_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * parent_item = lv_xml_state_get_parent(state);
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];
    }
}

void * wd_panel_dot_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_xml_state_get_parent(state);

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wd_panel-dot");
        return NULL;
    }

    return wd_panel_get_dot(item);
}

void wd_panel_dot_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * parent_item = lv_xml_state_get_parent(state);
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];
    }
}

void wd_panel_register(void)
{
    lv_xml_register_widget("wd_panel", wd_panel_xml_create, wd_panel_xml_apply);
    lv_xml_register_widget("wd_panel-container", wd_panel_container_xml_create, wd_panel_container_xml_apply);
    lv_xml_register_widget("wd_panel-dot", wd_panel_dot_xml_create, wd_panel_dot_xml_apply);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */