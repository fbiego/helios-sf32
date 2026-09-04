/**
 * @file wd_arcoiris_xml_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_arcoiris_gen.h"

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

void * wd_arcoiris_xml_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = wd_arcoiris_create(lv_xml_state_get_parent(state));

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create wd_arcoiris");
        return NULL;
    }

    return item;
}

void wd_arcoiris_xml_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("thickness", name)) 
        {
            wd_arcoiris_set_thickness(item, lv_xml_to_size(value));
        }
    }
}

void wd_arcoiris_register(void)
{
    lv_xml_register_widget("wd_arcoiris", wd_arcoiris_xml_create, wd_arcoiris_xml_apply);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */