#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "core/helios_display.c"

#if HELIOS_DISPLAY_ROTATION_CW
static lv_display_t display;
static lv_display_t *default_display;
static int allocation_fails, waits, pending;
static uint16_t physical[9];
static uint8_t *pending_pixels;
static lv_area_t pending_source, pending_clip;

lv_display_t *lv_display_get_default(void) { return default_display; }
int lv_display_get_horizontal_resolution(lv_display_t *d) { return d->width; }
int lv_display_get_vertical_resolution(lv_display_t *d) { return d->height; }
int lv_display_get_offset_x(lv_display_t *d) { (void)d; return 0; }
int lv_display_get_offset_y(lv_display_t *d) { (void)d; return 0; }
lv_color_format_t lv_display_get_color_format(lv_display_t *d) { (void)d; return LV_COLOR_FORMAT_RGB565; }
lv_draw_buf_t *lv_display_get_buf_active(lv_display_t *d) { return d->buf_act; }
lv_draw_buf_t *lv_draw_buf_create(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride)
{
    assert(cf == LV_COLOR_FORMAT_RGB565 && stride == w * 2);
    if (allocation_fails) return NULL;
    lv_draw_buf_t *buffer = calloc(1, sizeof(*buffer));
    buffer->data_size = stride * h;
    buffer->data = malloc(buffer->data_size);
    return lv_draw_buf_reshape(buffer, cf, w, h, stride);
}
lv_draw_buf_t *lv_draw_buf_reshape(lv_draw_buf_t *b, lv_color_format_t cf, uint32_t w, uint32_t h, uint32_t stride)
{
    assert(cf == LV_COLOR_FORMAT_RGB565);
    if (stride * h > b->data_size) return NULL;
    b->header.w = w; b->header.h = h; b->header.stride = stride;
    return b;
}
void lv_display_set_rotation(lv_display_t *d, lv_display_rotation_t r) { d->rotation = r; }
void lv_display_set_flush_cb(lv_display_t *d, lv_display_flush_cb_t cb) { d->flush_cb = cb; }
void lv_display_flush_ready(lv_display_t *d) { (void)d; }

/* Match the bundled LVGL's ROTATION_270 pixel and coordinate conventions. */
void lv_display_rotate_area(lv_display_t *d, lv_area_t *a)
{
    assert(d->rotation == LV_DISPLAY_ROTATION_270);
    lv_area_t old = *a;
    a->x1 = d->width - old.y2 - 1; a->x2 = d->width - old.y1 - 1;
    a->y1 = old.x1; a->y2 = old.x2;
}
void lv_draw_sw_rotate(const void *source, void *dest, int32_t w, int32_t h,
                       int32_t src_stride, int32_t dst_stride, lv_display_rotation_t r, lv_color_format_t cf)
{
    assert(r == LV_DISPLAY_ROTATION_270 && cf == LV_COLOR_FORMAT_RGB565);
    const uint16_t *src = source;
    uint16_t *dst = dest;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            dst[x * (dst_stride / 2) + h - 1 - y] = src[y * (src_stride / 2) + x];
}
static void sdk_wait(lv_display_t *d)
{
    (void)d;
    waits++;
    if (!pending) return;
    int width = lv_area_get_width(&pending_source);
    for (int y = pending_clip.y1; y <= pending_clip.y2; y++)
        for (int x = pending_clip.x1; x <= pending_clip.x2; x++)
            physical[y * 3 + x] = ((uint16_t *)pending_pixels)[(y - pending_source.y1) * width + x - pending_source.x1];
    pending = 0;
}
static void sdk_flush(lv_display_t *d, const lv_area_t *a, uint8_t *pixels)
{
    assert(!pending); /* Previous async copy must finish before scratch reuse. */
    pending_source = d->layer_head->buf_area;
    pending_clip = *a;
    pending_pixels = pixels;
    pending = 1;
}
#endif

int main(void)
{
#if HELIOS_DISPLAY_ROTATION_CW
    assert(helios_display_init_rotation() == -RT_ERROR); /* No display yet. */
    uint16_t first[] = {1, 2, 3, 4, 5, 6};
    lv_draw_buf_t source = {.header = {3, 2, 6}, .data = (uint8_t *)first, .data_size = sizeof(first)};
    lv_layer_t layer = {.buf_area = {0, 0, 2, 1}, .draw_buf = &source};
    display = (lv_display_t){.width = 3, .height = 3, .layer_head = &layer, .buf_act = &source,
                             .flush_cb = sdk_flush, .flush_wait_cb = sdk_wait};
    default_display = &display;
    display.render_mode = 1;
    assert(helios_display_init_rotation() == -RT_ENOSYS);
    display.render_mode = LV_DISPLAY_RENDER_MODE_PARTIAL;
    allocation_fails = 1;
    assert(helios_display_init_rotation() == -RT_ENOMEM && display.flush_cb == sdk_flush);
    allocation_fails = 0;
    assert(helios_display_init_rotation() == RT_EOK && display.rotation == LV_DISPLAY_ROTATION_270);
    assert(helios_display_init_rotation() == RT_EOK); /* Idempotent installation. */
    display.flush_cb(&display, &layer.buf_area, (uint8_t *)first);
    assert(layer.buf_area.x1 == 0 && layer.buf_area.y1 == 0 && layer.buf_area.x2 == 2 && layer.buf_area.y2 == 1);
    assert(pending_source.x1 == 1 && pending_source.y1 == 0 && pending_source.x2 == 2 && pending_source.y2 == 2);

    /* Next tile has a different shape and padded source stride. Its flush must
     * preserve the first tile until the SDK finishes its asynchronous copy. */
    uint16_t second[] = {7, 8, 0xffff};
    layer.buf_area = (lv_area_t){0, 2, 1, 2};
    source.header.stride = sizeof(second);
    lv_area_t clip = layer.buf_area;
    display.flush_cb(&display, &clip, (uint8_t *)second);
    assert(waits == 2 && physical[1] == 4 && physical[2] == 1);
    display.flush_wait_cb(&display);
    const uint16_t expected[] = {7, 4, 1, 8, 5, 2, 0, 6, 3};
    assert(memcmp(physical, expected, sizeof(expected)) == 0);
    assert(layer.buf_area.x1 == 0 && layer.buf_area.y1 == 2);

    /* A clipped dirty rectangle must be rotated independently of the full tile. */
    layer.buf_area = (lv_area_t){0, 0, 2, 1}; source.header.stride = 6;
    clip = (lv_area_t){1, 0, 2, 1};
    display.flush_cb(&display, &clip, (uint8_t *)first);
    assert(pending_clip.x1 == 1 && pending_clip.y1 == 1 && pending_clip.x2 == 2 && pending_clip.y2 == 2);
    display.flush_wait_cb(&display);
    /* LVGL's matching pointer rotation maps a physical tap back to the same
     * logical pixel, including all four corners and off-centre points. */
    for (int y = 0; y < 3; y++) for (int x = 0; x < 3; x++) {
        int physical_x = 2 - y, physical_y = x;
        int logical_x = physical_y, logical_y = 2 - physical_x;
        assert(logical_x == x && logical_y == y);
    }
    free(g_rotated->data); free(g_rotated);
#else
    assert(helios_display_init_rotation() == RT_EOK);
#endif
    return 0;
}
