/**
 * @file wd_obj_gen.h
 *
 */

#ifndef LVGL_PRO_WD_OBJ_GEN_H
#define LVGL_PRO_WD_OBJ_GEN_H

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
 * Create a wd_obj object
 * @param parent pointer to an object, it will be the parent of the new wd_obj
 * @return pointer to the created wd_obj
 */
lv_obj_t * wd_obj_create(lv_obj_t * parent);
/**
 * wd_obj bind_bg_color
 * @param obj   pointer to a wd_obj
 * @param bind_bg_color  bind_bg_color
 */
void wd_obj_bind_bg_color(lv_obj_t * wd_obj, lv_subject_t * bind_bg_color);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_OBJ_GEN_H*/