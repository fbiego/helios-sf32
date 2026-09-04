/**
 * @file hs_info_sb_sb_gen.h
 */

#ifndef LVGL_PRO_HS_INFO_SB_SB_GEN_H
#define LVGL_PRO_HS_INFO_SB_SB_GEN_H

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

lv_obj_t * hs_info_sb_sb_create(lv_obj_t * parent, lv_subject_t * label, lv_subject_t * info, int32_t info_space);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_HS_INFO_SB_SB_GEN_H*/