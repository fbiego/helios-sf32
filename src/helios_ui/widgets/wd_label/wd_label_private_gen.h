/**
 * @file wd_label_private_gen.h
 *
 */

#ifndef LVGL_PRO_WD_LABEL_PRIVATE_GEN_H
#define LVGL_PRO_WD_LABEL_PRIVATE_GEN_H

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
#include "wd_label.h"

/*********************
 *      DEFINES
 *********************/

#ifndef WD_LABEL_USER_DATA
#define WD_LABEL_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_label_t obj;  /* Base widget to extend */
    const char * text;
    const char * tag;
    lv_subject_t * bind_tag;
    lv_subject_t * bind_text;
    WD_LABEL_USER_DATA
} wd_label_t;

extern const lv_obj_class_t wd_label_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void wd_label_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_LABEL_PRIVATE_GEN_H*/