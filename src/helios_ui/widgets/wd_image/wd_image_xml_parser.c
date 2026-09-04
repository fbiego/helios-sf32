/**
 * @file wd_image_xml_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_image_gen.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "src/lvgl_private.h"
    #include "src/core/lv_obj_class_private.h"
#else
    #include "lvgl/src/lvgl_private.h"
    #include "lvgl/src/core/lv_obj_class_private.h"
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

void * wd_image_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = wd_image_create(lv_xml_state_get_parent(state));

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wd_image");
        return NULL;
    }

    return item;
}

void wd_image_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("src", name)) {
            wd_image_set_src(item, lv_xml_get_image(&state->scope, value));
        }
        else if(lv_streq("scale_0", name)) {
            wd_image_set_scale_0(item, lv_xml_atoi(value));
        }
        else if(lv_streq("scale_1", name)) {
            wd_image_set_scale_1(item, lv_xml_atoi(value));
        }
        else if(lv_streq("scale_2", name)) {
            wd_image_set_scale_2(item, lv_xml_atoi(value));
        }
        else if(lv_streq("size_0", name)) {
            wd_image_set_size_0(item, lv_xml_to_size(value));
        }
        else if(lv_streq("size_1", name)) {
            wd_image_set_size_1(item, lv_xml_to_size(value));
        }
        else if(lv_streq("size_2", name)) {
            wd_image_set_size_2(item, lv_xml_to_size(value));
        }
        else if(lv_streq("x_0", name)) {
            wd_image_set_x_0(item, lv_xml_to_size(value));
        }
        else if(lv_streq("x_1", name)) {
            wd_image_set_x_1(item, lv_xml_to_size(value));
        }
        else if(lv_streq("x_2", name)) {
            wd_image_set_x_2(item, lv_xml_to_size(value));
        }
        else if(lv_streq("rotation", name)) {
            wd_image_set_rotation(item, lv_xml_atoi(value));
        }
        else if(lv_streq("bind_scale", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                wd_image_bind_scale(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in wd_image bind_scale", value);
            }
        } else if(lv_streq("bind_src", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                wd_image_bind_src(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in wd_image bind_src", value);
            }
        } else if(lv_streq("bind_color", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                wd_image_bind_color(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in wd_image bind_color", value);
            }
        }
    }
}

void wd_image_register(void)
{
    lv_xml_register_widget("wd_image", wd_image_xml_create, wd_image_xml_apply);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */