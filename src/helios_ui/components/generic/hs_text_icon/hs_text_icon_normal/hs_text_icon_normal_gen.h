/**
 * @file hs_text_icon_normal_gen.h
 */

#ifndef LVGL_PRO_HS_TEXT_ICON_NORMAL_GEN_H
#define LVGL_PRO_HS_TEXT_ICON_NORMAL_GEN_H

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

lv_obj_t * hs_text_icon_normal_create(lv_obj_t * parent, const void * icon, const char * text, const char * tag, lv_subject_t * bind_value, const char * bind_fmt);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_HS_TEXT_ICON_NORMAL_GEN_H*/