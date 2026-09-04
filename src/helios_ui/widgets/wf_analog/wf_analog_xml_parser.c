/**
 * @file wf_analog_xml_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wf_analog_gen.h"

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

void * wf_analog_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = wf_analog_create(lv_xml_state_get_parent(state));

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wf_analog");
        return NULL;
    }

    return item;
}

void wf_analog_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("pivot_x", name)) {
            wf_analog_set_pivot_x(item, lv_xml_atoi(value));
        }

        if(lv_streq("pivot_y", name)) {
            wf_analog_set_pivot_y(item, lv_xml_atoi(value));
        }

        if(lv_streq("src", name)) {
            wf_analog_set_src(item, lv_xml_get_image(&state->scope, value));
        }

        if(lv_streq("offset", name)) {
            wf_analog_set_offset(item, lv_xml_atoi(value));
        }

        if(lv_streq("bind_rotation", name)) {
            wf_analog_bind_rotation(item, lv_xml_get_subject(&state->scope, value));
        }
    }
}

void wf_analog_register(void)
{
    lv_xml_register_widget("wf_analog", wf_analog_xml_create, wf_analog_xml_apply);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */