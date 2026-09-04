/**
 * @file wf_analog_private_gen.h
 *
 */

#ifndef LVGL_PRO_WF_ANALOG_PRIVATE_GEN_H
#define LVGL_PRO_WF_ANALOG_PRIVATE_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl_private.h"
#endif
#include "wf_analog.h"

/*********************
 *      DEFINES
 *********************/

#ifndef WF_ANALOG_USER_DATA
#define WF_ANALOG_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_image_t obj;  /* Base widget to extend */
    lv_subject_t * bind_rotation;
    const void * src;
    int32_t pivot_x;
    int32_t pivot_y;
    int32_t offset;
    WF_ANALOG_USER_DATA
} wf_analog_t;

extern const lv_obj_class_t wf_analog_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void wf_analog_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WF_ANALOG_PRIVATE_GEN_H*/