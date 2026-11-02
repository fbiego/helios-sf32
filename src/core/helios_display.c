#include "helios_display.h"
#include "helios_features.h"

#if HELIOS_DISPLAY_ROTATION_CW != 0
#if HELIOS_DISPLAY_ROTATION_CW != 90
#error Only the SFWatch 90-degree clockwise mounting offset is implemented
#endif

#include "lvgl.h"
#include "lvgl_private.h"
#include "src/draw/sw/lv_draw_sw_utils.h"

static lv_display_t *g_display;
static lv_draw_buf_t *g_rotated;
static lv_display_flush_cb_t g_original_flush;
static lv_display_flush_wait_cb_t g_original_wait;

static void helios_rotated_flush(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    /* The SDK callback owns DMA/framebuffer completion. Wait before reusing our
     * single scratch buffer, then keep its original flush/wait callbacks. */
    g_original_wait(display);

    lv_layer_t *layer = display->layer_head;
    lv_area_t original_source = layer->buf_area;
    lv_area_t rotated_source = original_source;
    lv_area_t rotated_clip = *area;
    int32_t width = lv_area_get_width(&original_source);
    int32_t height = lv_area_get_height(&original_source);
    uint32_t stride = (uint32_t)height * 2;

    lv_draw_buf_t *reshaped = lv_draw_buf_reshape(g_rotated, LV_COLOR_FORMAT_RGB565, height, width, stride);
    RT_ASSERT(reshaped != NULL);
    if (!reshaped)
    {
        lv_display_flush_ready(display);
        return;
    }

    /* In the bundled LVGL implementation ROTATION_270 maps pixels as
     * (x,y)->(height-1-y,x): clockwise on the physical panel. Its input-device
     * transform is the inverse, so buttons and gestures remain aligned. */
    lv_draw_sw_rotate(pixels, g_rotated->data, width, height,
                      layer->draw_buf->header.stride, stride,
                      LV_DISPLAY_ROTATION_270, LV_COLOR_FORMAT_RGB565);
    lv_display_rotate_area(display, &rotated_source);
    lv_display_rotate_area(display, &rotated_clip);

    /* The SDK derives the pixel source rectangle from layer->buf_area rather
     * than the flush rectangle. Supply its rotated counterpart for this call.
     * drv_lcd_fb_write_send consumes/copies the rectangle before returning. */
    layer->buf_area = rotated_source;
    g_original_flush(display, &rotated_clip, g_rotated->data);
    layer->buf_area = original_source;
}
#endif

rt_err_t helios_display_init_rotation(void)
{
#if HELIOS_DISPLAY_ROTATION_CW != 0
    lv_display_t *display = lv_display_get_default();
    if (!display)
        return -RT_ERROR;
    if (display == g_display)
        return RT_EOK;
    /* This adapter follows the SiFli partial-buffer RGB565 flush contract. */
    if (display->render_mode != LV_DISPLAY_RENDER_MODE_PARTIAL ||
        lv_display_get_color_format(display) != LV_COLOR_FORMAT_RGB565 ||
        lv_display_get_horizontal_resolution(display) != lv_display_get_vertical_resolution(display) ||
        lv_display_get_offset_x(display) || lv_display_get_offset_y(display) ||
        !display->flush_cb || !display->flush_wait_cb)
        return -RT_ENOSYS;

    lv_draw_buf_t *source = lv_display_get_buf_active(display);
    if (!source)
        return -RT_ERROR;
    g_rotated = lv_draw_buf_create(source->header.h, source->header.w,
                                   LV_COLOR_FORMAT_RGB565, source->header.h * 2);
    if (!g_rotated)
        return -RT_ENOMEM;
    g_original_flush = display->flush_cb;
    g_original_wait = display->flush_wait_cb;
    g_display = display;
    lv_display_set_rotation(display, LV_DISPLAY_ROTATION_270);
    lv_display_set_flush_cb(display, helios_rotated_flush);
#endif
    return RT_EOK;
}
