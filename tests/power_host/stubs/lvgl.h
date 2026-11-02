#ifndef POWER_TEST_LVGL_H
#define POWER_TEST_LVGL_H
#include <stdbool.h>
typedef struct { int paused; } lv_timer_t;
typedef struct lv_display_t lv_display_t;
struct lv_display_t { void (*flush_wait_cb)(lv_display_t *); };
lv_display_t *lv_display_get_default(void);
lv_timer_t *lv_display_get_refr_timer(lv_display_t *);
void lv_display_enable_invalidation(lv_display_t *, bool);
void lv_timer_pause(lv_timer_t *);
void lv_timer_resume(lv_timer_t *);
void lv_timer_ready(lv_timer_t *);
#endif
