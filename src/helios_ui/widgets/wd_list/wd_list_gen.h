/**
 * @file wd_list_gen.h
 *
 */

#ifndef LVGL_PRO_WD_LIST_GEN_H
#define LVGL_PRO_WD_LIST_GEN_H

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
 * Create a wd_list object
 * @param parent pointer to an object, it will be the parent of the new wd_list
 * @return pointer to the created wd_list
 */
lv_obj_t * wd_list_create(lv_obj_t * parent);
/**
 * Subject that changes between list and grid mode
 * @param obj   pointer to a wd_list
 * @param bind_mode  Subject that changes between list and grid mode
 */
void wd_list_bind_mode(lv_obj_t * wd_list, lv_subject_t * bind_mode);

/**
 * Subject that enables circular scroll in list mode
 * @param obj   pointer to a wd_list
 * @param bind_circular  Subject that enables circular scroll in list mode
 */
void wd_list_bind_circular(lv_obj_t * wd_list, lv_subject_t * bind_circular);

/**
 * Subject that enables circular scroll in list mode
 * @param obj   pointer to a wd_list
 * @param bind_screen  Subject that enables circular scroll in list mode
 */
void wd_list_bind_screen(lv_obj_t * wd_list, lv_subject_t * bind_screen);

/**
 * Center prevents scrolling in order to detect gesture
 * @param obj   pointer to a wd_list
 * @param center_lock  Center prevents scrolling in order to detect gesture
 */
void wd_list_set_center_lock(lv_obj_t * wd_list, bool center_lock);

/**
 * wd_list container
 * @param obj   pointer to a wd_list

 */
lv_obj_t * wd_list_get_container(lv_obj_t * wd_list);

/**
 * wd_list title
 * @param obj   pointer to a wd_list

 */
lv_obj_t * wd_list_get_title(lv_obj_t * wd_list);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_WD_LIST_GEN_H*/