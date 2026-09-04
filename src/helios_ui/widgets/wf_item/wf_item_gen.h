/**
 * @file wf_item_gen.h
 *
 */

#ifndef LVGL_PRO_WF_ITEM_GEN_H
#define LVGL_PRO_WF_ITEM_GEN_H

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
 * Create a wf_item object
 * @param parent pointer to an object, it will be the parent of the new wf_item
 * @return pointer to the created wf_item
 */
lv_obj_t * wf_item_create(lv_obj_t * parent);
/**
 * The array containing the image digits
 * @param obj   pointer to a wf_item
 * @param image_array  The array containing the image digits
 */
void wf_item_set_image_array(lv_obj_t * wf_item, const void * image_array[], uint32_t image_array_count);

/**
 * The watchface subject to attach the item
 * @param obj   pointer to a wf_item
 * @param bind_subject  The watchface subject to attach the item
 */
void wf_item_bind_subject(lv_obj_t * wf_item, lv_subject_t * bind_subject);

/**
 * The place value to show on this item
 * @param obj   pointer to a wf_item
 * @param place_value  The place value to show on this item
 */
void wf_item_set_place_value(lv_obj_t * wf_item, int32_t place_value);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WF_ITEM_GEN_H*/