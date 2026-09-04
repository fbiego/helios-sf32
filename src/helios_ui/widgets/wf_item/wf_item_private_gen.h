/**
 * @file wf_item_private_gen.h
 *
 */

#ifndef LVGL_PRO_WF_ITEM_PRIVATE_GEN_H
#define LVGL_PRO_WF_ITEM_PRIVATE_GEN_H

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
#include "wf_item.h"

/*********************
 *      DEFINES
 *********************/

#ifndef WF_ITEM_USER_DATA
#define WF_ITEM_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_image_t obj;  /* Base widget to extend */
    const void ** image_array;
    lv_subject_t * bind_subject;
    int32_t place_value;
    WF_ITEM_USER_DATA
} wf_item_t;

extern const lv_obj_class_t wf_item_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void wf_item_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WF_ITEM_PRIVATE_GEN_H*/