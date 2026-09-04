/**
 * @file wd_dropdown_gen.h
 *
 */

#ifndef LVGL_PRO_WD_DROPDOWN_GEN_H
#define LVGL_PRO_WD_DROPDOWN_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#include "../../helios_ui_gen.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a wd_dropdown object
 * @param parent pointer to an object, it will be the parent of the new wd_dropdown
 * @return pointer to the created wd_dropdown
 */
lv_obj_t * wd_dropdown_create(lv_obj_t * parent);
/**
 * wd_dropdown options
 * @param obj   pointer to a wd_dropdown
 * @param options  options
 */
void wd_dropdown_set_options(lv_obj_t * wd_dropdown, const char * options);

/**
 * The current selected item, -1 for none
 * @param obj   pointer to a wd_dropdown
 * @param selected  The current selected item, -1 for none
 */
void wd_dropdown_set_selected(lv_obj_t * wd_dropdown, int32_t selected);

/**
 * The subject to bind the selected item
 * @param obj   pointer to a wd_dropdown
 * @param bind_value  The subject to bind the selected item
 */
void wd_dropdown_bind_value(lv_obj_t * wd_dropdown, lv_subject_t * bind_value);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_DROPDOWN_GEN_H*/