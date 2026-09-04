/**
 * @file notification_item_gen.h
 */

#ifndef LVGL_PRO_NOTIFICATION_ITEM_GEN_H
#define LVGL_PRO_NOTIFICATION_ITEM_GEN_H

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

lv_obj_t * notification_item_create(lv_obj_t * parent, const void * icon, const char * title, const char * time, const char * message);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_NOTIFICATION_ITEM_GEN_H*/