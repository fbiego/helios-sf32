/**
 * @file info_dialog_gen.h
 */

#ifndef LVGL_PRO_INFO_DIALOG_GEN_H
#define LVGL_PRO_INFO_DIALOG_GEN_H

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

lv_obj_t * info_dialog_create(lv_obj_t * parent, const char * title, const char * info, const char * positive_btn, const char * negative_btn);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_INFO_DIALOG_GEN_H*/