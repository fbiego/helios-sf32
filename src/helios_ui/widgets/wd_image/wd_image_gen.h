/**
 * @file wd_image_gen.h
 *
 */

#ifndef LVGL_PRO_WD_IMAGE_GEN_H
#define LVGL_PRO_WD_IMAGE_GEN_H

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
 * Create a wd_image object
 * @param parent pointer to an object, it will be the parent of the new wd_image
 * @return pointer to the created wd_image
 */
lv_obj_t * wd_image_create(lv_obj_t * parent);
/**
 * wd_image src
 * @param obj   pointer to a wd_image
 * @param src  src
 */
void wd_image_set_src(lv_obj_t * wd_image, const void * src);

/**
 * wd_image scale_0
 * @param obj   pointer to a wd_image
 * @param scale_0  scale_0
 */
void wd_image_set_scale_0(lv_obj_t * wd_image, int32_t scale_0);

/**
 * wd_image scale_1
 * @param obj   pointer to a wd_image
 * @param scale_1  scale_1
 */
void wd_image_set_scale_1(lv_obj_t * wd_image, int32_t scale_1);

/**
 * wd_image scale_2
 * @param obj   pointer to a wd_image
 * @param scale_2  scale_2
 */
void wd_image_set_scale_2(lv_obj_t * wd_image, int32_t scale_2);

/**
 * wd_image size_0
 * @param obj   pointer to a wd_image
 * @param size_0  size_0
 */
void wd_image_set_size_0(lv_obj_t * wd_image, int32_t size_0);

/**
 * wd_image size_1
 * @param obj   pointer to a wd_image
 * @param size_1  size_1
 */
void wd_image_set_size_1(lv_obj_t * wd_image, int32_t size_1);

/**
 * wd_image size_2
 * @param obj   pointer to a wd_image
 * @param size_2  size_2
 */
void wd_image_set_size_2(lv_obj_t * wd_image, int32_t size_2);

/**
 * wd_image x_0
 * @param obj   pointer to a wd_image
 * @param x_0  x_0
 */
void wd_image_set_x_0(lv_obj_t * wd_image, int32_t x_0);

/**
 * wd_image x_1
 * @param obj   pointer to a wd_image
 * @param x_1  x_1
 */
void wd_image_set_x_1(lv_obj_t * wd_image, int32_t x_1);

/**
 * wd_image x_2
 * @param obj   pointer to a wd_image
 * @param x_2  x_2
 */
void wd_image_set_x_2(lv_obj_t * wd_image, int32_t x_2);

/**
 * wd_image rotation
 * @param obj   pointer to a wd_image
 * @param rotation  rotation
 */
void wd_image_set_rotation(lv_obj_t * wd_image, int32_t rotation);

/**
 * wd_image bind_scale
 * @param obj   pointer to a wd_image
 * @param bind_scale  bind_scale
 */
void wd_image_bind_scale(lv_obj_t * wd_image, lv_subject_t * bind_scale);

/**
 * wd_image bind_src
 * @param obj   pointer to a wd_image
 * @param bind_src  bind_src
 */
void wd_image_bind_src(lv_obj_t * wd_image, lv_subject_t * bind_src);

/**
 * wd_image bind_color
 * @param obj   pointer to a wd_image
 * @param bind_color  bind_color
 */
void wd_image_bind_color(lv_obj_t * wd_image, lv_subject_t * bind_color);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_IMAGE_GEN_H*/