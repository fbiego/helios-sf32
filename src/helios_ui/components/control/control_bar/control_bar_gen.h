/**
 * @file control_bar_gen.h
 */

#ifndef LVGL_PRO_CONTROL_BAR_GEN_H
#define LVGL_PRO_CONTROL_BAR_GEN_H

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

lv_obj_t * control_bar_create(lv_obj_t * parent, lv_subject_t * bind_subject, const char * bind_fmt, const void * icon, bool hide_text);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_CONTROL_BAR_GEN_H*/