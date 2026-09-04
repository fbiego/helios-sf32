/**
 * @file hs_button_icon_gen.h
 */

#ifndef LVGL_PRO_HS_BUTTON_ICON_GEN_H
#define LVGL_PRO_HS_BUTTON_ICON_GEN_H

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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * hs_button_icon_create(lv_obj_t * parent, const void * icon, int32_t rotation, lv_subject_t * bind_icon);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_HS_BUTTON_ICON_GEN_H*/