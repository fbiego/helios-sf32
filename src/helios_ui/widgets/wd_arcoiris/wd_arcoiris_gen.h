/**
 * @file wd_arcoiris_gen.h
 *
 */

#ifndef LVGL_PRO_WD_ARCOIRIS_GEN_H
#define LVGL_PRO_WD_ARCOIRIS_GEN_H

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
 * Create a wd_arcoiris object
 * @param parent pointer to an object, it will be the parent of the new wd_arcoiris
 * @return pointer to the created wd_arcoiris
 */
lv_obj_t * wd_arcoiris_create(lv_obj_t * parent);
/**
 * Thickness of the arc widget
 * @param obj   pointer to a wd_arcoiris
 * @param thickness  Thickness of the arc widget
 */
void wd_arcoiris_set_thickness(lv_obj_t * wd_arcoiris, int32_t thickness);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_ARCOIRIS_GEN_H*/