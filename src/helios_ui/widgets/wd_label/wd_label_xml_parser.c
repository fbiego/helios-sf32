/**
 * @file wd_label_xml_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_label_gen.h"

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

void * wd_label_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = wd_label_create(lv_xml_state_get_parent(state));

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wd_label");
        return NULL;
    }

    return item;
}

void wd_label_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];
        if(lv_streq("text", name)) {
           wd_label_set_text(item, value);
        }
        if(lv_streq("tag", name)) {
            wd_label_set_tag(item, value);
        }
        if(lv_streq("bind_text", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                wd_label_bind_text(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in wd_label bind_text", value);
            }
        }
        if(lv_streq("bind_tag", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                wd_label_bind_tag(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in wd_label bind_tag", value);
            }
        }
    }
}

void wd_label_register(void)
{
    lv_xml_register_widget("wd_label", wd_label_xml_create, wd_label_xml_apply);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */