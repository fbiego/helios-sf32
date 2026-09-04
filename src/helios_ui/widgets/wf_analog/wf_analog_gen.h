/**
 * @file wf_analog_gen.h
 *
 */

#ifndef LVGL_PRO_WF_ANALOG_GEN_H
#define LVGL_PRO_WF_ANALOG_GEN_H

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
 * Create a wf_analog object
 * @param parent pointer to an object, it will be the parent of the new wf_analog
 * @return pointer to the created wf_analog
 */
lv_obj_t * wf_analog_create(lv_obj_t * parent);
/**
 * The subject to bind the image rotation
 * @param obj   pointer to a wf_analog
 * @param bind_rotation  The subject to bind the image rotation
 */
void wf_analog_bind_rotation(lv_obj_t * wf_analog, lv_subject_t * bind_rotation);

/**
 * The image src
 * @param obj   pointer to a wf_analog
 * @param src  The image src
 */
void wf_analog_set_src(lv_obj_t * wf_analog, const void * src);

/**
 * The x pivot point of the image
 * @param obj   pointer to a wf_analog
 * @param pivot_x  The x pivot point of the image
 */
void wf_analog_set_pivot_x(lv_obj_t * wf_analog, int32_t pivot_x);

/**
 * The y pivot point of the image
 * @param obj   pointer to a wf_analog
 * @param pivot_y  The y pivot point of the image
 */
void wf_analog_set_pivot_y(lv_obj_t * wf_analog, int32_t pivot_y);

/**
 * The offset of the rotation
 * @param obj   pointer to a wf_analog
 * @param offset  The offset of the rotation
 */
void wf_analog_set_offset(lv_obj_t * wf_analog, int32_t offset);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WF_ANALOG_GEN_H*/