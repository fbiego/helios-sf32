/**
 * @file wd_label_gen.h
 *
 */

#ifndef LVGL_PRO_WD_LABEL_GEN_H
#define LVGL_PRO_WD_LABEL_GEN_H

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
 * Create a wd_label object
 * @param parent pointer to an object, it will be the parent of the new wd_label
 * @return pointer to the created wd_label
 */
lv_obj_t * wd_label_create(lv_obj_t * parent);
/**
 * wd_label text
 * @param obj   pointer to a wd_label
 * @param text  text
 */
void wd_label_set_text(lv_obj_t * wd_label, const char * text);

/**
 * wd_label tag
 * @param obj   pointer to a wd_label
 * @param tag  tag
 */
void wd_label_set_tag(lv_obj_t * wd_label, const char * tag);

/**
 * wd_label bind_tag
 * @param obj   pointer to a wd_label
 * @param bind_tag  bind_tag
 */
void wd_label_bind_tag(lv_obj_t * wd_label, lv_subject_t * bind_tag);

/**
 * wd_label bind_text
 * @param obj   pointer to a wd_label
 * @param bind_text  bind_text
 */
void wd_label_bind_text(lv_obj_t * wd_label, lv_subject_t * bind_text);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_LABEL_GEN_H*/