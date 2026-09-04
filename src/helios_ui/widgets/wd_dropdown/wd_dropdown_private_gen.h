/**
 * @file wd_dropdown_private_gen.h
 *
 */

#ifndef LVGL_PRO_WD_DROPDOWN_PRIVATE_GEN_H
#define LVGL_PRO_WD_DROPDOWN_PRIVATE_GEN_H

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
#include "wd_dropdown.h"

/*********************
 *      DEFINES
 *********************/

#ifndef WD_DROPDOWN_USER_DATA
#define WD_DROPDOWN_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;  /* Base widget to extend */
    const char * options;
    int32_t selected;
    lv_subject_t * bind_value;
    lv_obj_t * main_label;
    lv_obj_t * dropdown_cont;
    lv_obj_t * dropdown_list;
    WD_DROPDOWN_USER_DATA
} wd_dropdown_t;

extern const lv_obj_class_t wd_dropdown_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void wd_dropdown_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_DROPDOWN_PRIVATE_GEN_H*/