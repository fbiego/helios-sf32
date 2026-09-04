/**
 * @file watchface_previews.c
 * @brief Editor-only watchface preview asset registration
 */

/*********************
 *      INCLUDES
 *********************/

#include "watchface_manager.h"

/**********************
 *  EXTERN VARIABLES
 **********************/

extern const lv_image_dsc_t img_preview_240_data;
extern const lv_image_dsc_t img_preview_280_data;
extern const lv_image_dsc_t img_preview_360_data;
extern const lv_image_dsc_t img_preview_390_data;
extern const lv_image_dsc_t img_preview_410_data;
extern const lv_image_dsc_t img_preview_466_data;

const void * img_preview_240 = &img_preview_240_data;
const void * img_preview_280 = &img_preview_280_data;
const void * img_preview_360 = &img_preview_360_data;
const void * img_preview_390 = &img_preview_390_data;
const void * img_preview_410 = &img_preview_410_data;
const void * img_preview_466 = &img_preview_466_data;

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void default_watchface_previews_register(void)
{
    const helios_watchface_preview_t previews[] = {
        { 240, 240, img_preview_240 },
        { 240, 280, img_preview_280 },
        { 360, 360, img_preview_360 },
        { 390, 450, img_preview_390 },
        { 410, 502, img_preview_410 },
        { 466, 466, img_preview_466 },
    };

    helios_watchfaces_set_previews("default",
                                   previews,
                                   (uint32_t)(sizeof(previews) / sizeof(previews[0])));
}

HELIOS_REGISTER_WATCHFACE_INITIALIZER(default_watchface_previews_register)
