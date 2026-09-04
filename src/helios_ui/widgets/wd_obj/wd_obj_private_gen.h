/**
 * @file wd_obj_private_gen.h
 *
 */

#ifndef LVGL_PRO_WD_OBJ_PRIVATE_GEN_H
#define LVGL_PRO_WD_OBJ_PRIVATE_GEN_H

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
#include "wd_obj.h"

/*********************
 *      DEFINES
 *********************/

#ifndef WD_OBJ_USER_DATA
#define WD_OBJ_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;  /* Base widget to extend */
    lv_subject_t * bind_bg_color;
    WD_OBJ_USER_DATA
} wd_obj_t;

extern const lv_obj_class_t wd_obj_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void wd_obj_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_OBJ_PRIVATE_GEN_H*/