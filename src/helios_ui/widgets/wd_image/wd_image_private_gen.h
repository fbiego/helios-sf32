/**
 * @file wd_image_private_gen.h
 *
 */

#ifndef LVGL_PRO_WD_IMAGE_PRIVATE_GEN_H
#define LVGL_PRO_WD_IMAGE_PRIVATE_GEN_H

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
#include "wd_image.h"

/*********************
 *      DEFINES
 *********************/

#ifndef WD_IMAGE_USER_DATA
#define WD_IMAGE_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_image_t obj;  /* Base widget to extend */
    const void * src;
    int32_t scale_0;
    int32_t scale_1;
    int32_t scale_2;
    int32_t size_0;
    int32_t size_1;
    int32_t size_2;
    int32_t x_0;
    int32_t x_1;
    int32_t x_2;
    int32_t rotation;
    lv_subject_t * bind_scale;
    lv_subject_t * bind_src;
    lv_subject_t * bind_color;
    WD_IMAGE_USER_DATA
} wd_image_t;

extern const lv_obj_class_t wd_image_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void wd_image_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_IMAGE_PRIVATE_GEN_H*/