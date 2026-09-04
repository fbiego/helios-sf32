
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
#include <lvgl.h>
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_ICON_EMPTY_DATA
#define LV_ATTRIBUTE_ICON_EMPTY_DATA
#endif

static const
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_ICON_EMPTY_DATA
uint8_t icon_empty_data_map[] = {

    0x00,

};

const lv_image_dsc_t icon_empty_data = {
  .header = {
    .magic = LV_IMAGE_HEADER_MAGIC,
    .cf = LV_COLOR_FORMAT_A8,
    .flags = 0,
    .w = 1,
    .h = 1,
    .stride = 1,
    .reserved_2 = 0,
  },
  .data_size = sizeof(icon_empty_data_map),
  .data = icon_empty_data_map,
  .reserved = NULL,
};

