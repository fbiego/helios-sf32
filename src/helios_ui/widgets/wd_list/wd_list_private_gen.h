/**
 * @file wd_list_private_gen.h
 *
 */

#ifndef LVGL_PRO_WD_LIST_PRIVATE_GEN_H
#define LVGL_PRO_WD_LIST_PRIVATE_GEN_H

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
#include "wd_list.h"

/*********************
 *      DEFINES
 *********************/

#ifndef WD_LIST_USER_DATA
#define WD_LIST_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;  /* Base widget to extend */
    lv_subject_t * bind_mode;
    lv_subject_t * bind_circular;
    lv_subject_t * bind_screen;
    bool center_lock;
    lv_obj_t * list;
    lv_obj_t * title;
    WD_LIST_USER_DATA
} wd_list_t;

extern const lv_obj_class_t wd_list_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void wd_list_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_LIST_PRIVATE_GEN_H*/