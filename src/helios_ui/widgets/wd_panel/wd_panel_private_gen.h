/**
 * @file wd_panel_private_gen.h
 *
 */

#ifndef LVGL_PRO_WD_PANEL_PRIVATE_GEN_H
#define LVGL_PRO_WD_PANEL_PRIVATE_GEN_H

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
#include "wd_panel.h"

/*********************
 *      DEFINES
 *********************/

#ifndef WD_PANEL_USER_DATA
#define WD_PANEL_USER_DATA
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;  /* Base widget to extend */
    lv_obj_t * container;
    lv_obj_t * dots;
    WD_PANEL_USER_DATA
} wd_panel_t;

extern const lv_obj_class_t wd_panel_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if defined(LV_USE_XML) && LV_USE_XML
    void wd_panel_register(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_PANEL_PRIVATE_GEN_H*/