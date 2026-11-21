#ifndef BATTERY_UI_TEST_LVGL_H
#define BATTERY_UI_TEST_LVGL_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#define LV_EVENT_DELETE 1
#define LV_CHART_POINT_NONE INT32_MAX
typedef struct lv_obj_t lv_obj_t;
typedef struct { lv_obj_t *target; } lv_event_t;
typedef struct { int unused; } lv_chart_series_t;
typedef struct lv_timer_t {
    void (*callback)(struct lv_timer_t *);
    bool deleted;
} lv_timer_t;
struct lv_obj_t {
    lv_obj_t *child;
    void (*on_delete)(lv_event_t *);
    unsigned count, renders;
    int32_t values[25];
    bool deleted;
};
lv_obj_t *lv_obj_find_by_name(lv_obj_t *, const char *);
void lv_obj_add_event_cb(lv_obj_t *, void (*)(lv_event_t *), int, void *);
lv_obj_t *lv_event_get_target(lv_event_t *);
lv_chart_series_t *lv_chart_get_series_next(lv_obj_t *, lv_chart_series_t *);
void lv_chart_set_point_count(lv_obj_t *, uint32_t);
void lv_chart_set_series_values(lv_obj_t *, lv_chart_series_t *, const int32_t *, size_t);
void lv_chart_refresh(lv_obj_t *);
uint32_t lv_tick_get(void);
uint32_t lv_tick_elaps(uint32_t);
lv_timer_t *lv_timer_create(void (*)(lv_timer_t *), uint32_t, void *);
void lv_timer_delete(lv_timer_t *);
#endif
