/**
 * @file hs_info_tag_sb_gen.h
 */

#ifndef LVGL_PRO_HS_INFO_TAG_SB_GEN_H
#define LVGL_PRO_HS_INFO_TAG_SB_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
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

lv_obj_t * hs_info_tag_sb_create(lv_obj_t * parent, const char * label, const char * label_tag, lv_subject_t * info);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_HS_INFO_TAG_SB_GEN_H*/