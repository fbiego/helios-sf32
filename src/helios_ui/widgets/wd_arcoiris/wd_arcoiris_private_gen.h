/**
 * @file wd_arcoiris_private_gen.h
 *
 */

#ifndef LVGL_PRO_WD_ARCOIRIS_PRIVATE_GEN_H
#define LVGL_PRO_WD_ARCOIRIS_PRIVATE_GEN_H

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
#include "wd_arcoiris.h"

/*********************
 *      DEFINES
 *********************/

#ifndef WD_ARCOIRIS_USER_DATA
#define WD_ARCOIRIS_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;  /* Base widget to extend */
    int32_t thickness;
    WD_ARCOIRIS_USER_DATA
} wd_arcoiris_t;

extern const lv_obj_class_t wd_arcoiris_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void wd_arcoiris_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_ARCOIRIS_PRIVATE_GEN_H*/