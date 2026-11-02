#ifndef DISPLAY_TEST_LVGL_H
#define DISPLAY_TEST_LVGL_H
#include <stdint.h>
#include <stddef.h>
typedef struct lv_display_t lv_display_t;
typedef struct { int32_t x1, y1, x2, y2; } lv_area_t;
typedef struct {
    struct { uint32_t w, h, stride; } header;
    uint8_t *data;
    uint32_t data_size;
} lv_draw_buf_t;
typedef struct { lv_area_t buf_area; lv_draw_buf_t *draw_buf; } lv_layer_t;
typedef int lv_display_rotation_t;
typedef int lv_color_format_t;
typedef void (*lv_display_flush_cb_t)(lv_display_t *, const lv_area_t *, uint8_t *);
typedef void (*lv_display_flush_wait_cb_t)(lv_display_t *);
#define LV_DISPLAY_ROTATION_270 3
#define LV_DISPLAY_RENDER_MODE_PARTIAL 0
#define LV_COLOR_FORMAT_RGB565 18
struct lv_display_t {
    int render_mode, width, height, rotation;
    lv_layer_t *layer_head;
    lv_draw_buf_t *buf_act;
    lv_display_flush_cb_t flush_cb;
    lv_display_flush_wait_cb_t flush_wait_cb;
};
static inline int32_t lv_area_get_width(const lv_area_t *a) { return a->x2 - a->x1 + 1; }
static inline int32_t lv_area_get_height(const lv_area_t *a) { return a->y2 - a->y1 + 1; }
lv_display_t *lv_display_get_default(void);
int lv_display_get_horizontal_resolution(lv_display_t *);
int lv_display_get_vertical_resolution(lv_display_t *);
int lv_display_get_offset_x(lv_display_t *);
int lv_display_get_offset_y(lv_display_t *);
lv_color_format_t lv_display_get_color_format(lv_display_t *);
lv_draw_buf_t *lv_display_get_buf_active(lv_display_t *);
lv_draw_buf_t *lv_draw_buf_create(uint32_t, uint32_t, lv_color_format_t, uint32_t);
lv_draw_buf_t *lv_draw_buf_reshape(lv_draw_buf_t *, lv_color_format_t, uint32_t, uint32_t, uint32_t);
void lv_display_set_rotation(lv_display_t *, lv_display_rotation_t);
void lv_display_set_flush_cb(lv_display_t *, lv_display_flush_cb_t);
void lv_display_rotate_area(lv_display_t *, lv_area_t *);
void lv_display_flush_ready(lv_display_t *);
#endif
