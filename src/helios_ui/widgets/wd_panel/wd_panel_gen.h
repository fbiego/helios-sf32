/**
 * @file wd_panel_gen.h
 *
 */

#ifndef LVGL_PRO_WD_PANEL_GEN_H
#define LVGL_PRO_WD_PANEL_GEN_H

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

#include "../../helios_ui_gen.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a wd_panel object
 * @param parent pointer to an object, it will be the parent of the new wd_panel
 * @return pointer to the created wd_panel
 */
lv_obj_t * wd_panel_create(lv_obj_t * parent);
/**
 * wd_panel container
 * @param obj   pointer to a wd_panel

 */
lv_obj_t * wd_panel_get_container(lv_obj_t * wd_panel);

/**
 * wd_panel dot
 * @param obj   pointer to a wd_panel

 */
lv_obj_t * wd_panel_get_dot(lv_obj_t * wd_panel);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_PANEL_GEN_H*/