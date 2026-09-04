/**
 * @file wf_item_xml_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wf_item_gen.h"

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

void * wf_item_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = wf_item_create(lv_xml_state_get_parent(state));

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wf_item");
        return NULL;
    }

    return item;
}

void wf_item_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if (lv_streq("place_value", name)) {
            wf_item_set_place_value(item, lv_xml_atoi(value));
        }
        if (lv_streq("bind_subject", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                wf_item_bind_subject(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in wd_item bind_subject", value);
            }
        }
        if (lv_streq("image_array", name)) {
            
            // wf_item_set_image_array(item,  ,);
        }
    }
}

void wf_item_register(void)
{
    lv_xml_register_widget("wf_item", wf_item_xml_create, wf_item_xml_apply);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */