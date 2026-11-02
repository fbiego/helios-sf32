/* Drive real LVGL pointer events without a window or physical touch device. */
#include "helios_ui.h"
#include "custom/apps/app_screens.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>

static lv_indev_t * pointer;
static lv_indev_data_t sample;

static void read_pointer(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    *data = sample;
}

static void touch(int32_t x, int32_t y, bool pressed, uint32_t elapsed)
{
    sample.point = (lv_point_t){x, y};
    sample.state = pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    lv_tick_inc(elapsed);
    lv_obj_update_layout(lv_screen_active());
    lv_indev_read(pointer);
    lv_obj_update_layout(lv_screen_active());
}

static void settle(void)
{
    for (int i = 0; i < 20; i++) {
        lv_tick_inc(20);
        lv_timer_handler();
    }
    lv_obj_update_layout(lv_screen_active());
    lv_timer_pause(lv_display_get_refr_timer(lv_display_get_default()));
}

static lv_obj_t * new_home(void)
{
    touch(0, 0, false, 20);
    lv_obj_t * old = lv_screen_active();
    lv_obj_t * home = screen_home();
    lv_screen_load(home);
    lv_obj_delete(old);
    lv_obj_update_layout(home);
    return home;
}

static lv_obj_t * panel(lv_obj_t * home)
{
    lv_obj_t * handle = lv_obj_find_by_name(home, "drag_control");
    assert(handle);
    return lv_obj_get_parent(handle);
}

static void assert_hidden(lv_obj_t * home)
{
    assert(lv_obj_get_y_aligned(panel(home)) == -lv_obj_get_height(home));
}

static void check_held_drag(int32_t distance, bool should_open)
{
    lv_obj_t * home = new_home();
    touch(230, 100, true, 20);
    touch(230, 100 + distance, true, 20);
    int32_t held_y = lv_obj_get_y_aligned(panel(home));
    assert(held_y == -lv_obj_get_height(home) + distance);

    /* A moving child must not take over an in-progress home-screen drag. */
    lv_obj_t * child = lv_obj_create(home);
    lv_obj_set_pos(child, 200, 90 + distance);
    lv_obj_set_size(child, 60, 60);
    for (int i = 0; i < 25; i++) {
        touch(230, 100 + distance, true, 20);
        settle();
        assert(lv_screen_active() == home);
        assert(lv_obj_get_y_aligned(panel(home)) == held_y);
        lv_area_t area;
        lv_obj_get_coords(panel(home), &area);
        assert(area.y1 == held_y);
    }
    lv_obj_delete(child);
    touch(230, 100 + distance, false, 20);
    settle();
    int32_t target_y = should_open ? 0 : -lv_obj_get_height(home);
    assert(lv_obj_get_y_aligned(panel(home)) == target_y);
}

int main(void)
{
    lv_init();
    lv_display_t * display = lv_display_create(466, 466);
    /* Rendering isn't needed; keep animations/timers deterministic. */
    lv_timer_pause(lv_display_get_refr_timer(display));
    helios_ui_init("A:");
    pointer = lv_indev_create();
    lv_indev_set_type(pointer, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(pointer, read_pointer);
    lv_indev_set_mode(pointer, LV_INDEV_MODE_EVENT);
    lv_indev_set_long_press_time(pointer, 400);

    check_held_drag(60, false);
    check_held_drag(120, true);
    puts("PASS: held drags stay under the finger for 10 seconds and settle only on release");

    lv_obj_t * home = new_home();
    touch(230, 160, true, 20);
    for (int y = 161; y <= 280; y++) {
        touch(230, y, true, 20); /* Slower than LVGL's gesture velocity threshold. */
        assert(lv_screen_active() == home);
    }
    assert(lv_obj_get_y_aligned(panel(home)) > -466);
    touch(230, 280, false, 20);
    assert(lv_screen_active() == home);
    settle();
    assert(lv_obj_get_y_aligned(panel(home)) == 0);
    puts("PASS: slow downward drag suppresses watchface selection");

    touch(230, 430, true, 20);
    touch(230, 430, true, 450);
    assert(lv_screen_active() == home);
    touch(230, 280, true, 20);
    touch(230, 280, false, 20);
    settle();
    assert_hidden(home);
    puts("PASS: panel handle closes the panel and cannot open watchface selection");

    for (int direction = -1; direction <= 1; direction += 2) {
        home = new_home();
        touch(230, 160, true, 20);
        for (int d = 1; d <= 100; d++) {
            touch(230 + direction * d, 160 + d / 4, true, 20);
            assert(lv_screen_active() == home);
            assert_hidden(home);
        }
        /* Direction stays locked even if the finger then moves down. */
        touch(230 + direction * 100, 310, true, 20);
        assert_hidden(home);
        touch(230 + direction * 100, 310, false, 20);
    }
    puts("PASS: horizontal drags with vertical drift never reveal the panel");

    for (int direction = -1; direction <= 1; direction += 2) {
        home = new_home();
        touch(230, 160, true, 20);
        touch(230 + direction * 20, 164, true, 20);
        assert_hidden(home);
        touch(230 + direction * 100, 170, true, 20);
        assert_hidden(home);
        touch(230 + direction * 100, 170, false, 20);
        settle();
        assert(lv_screen_active() != home);
        const char * list = direction < 0 ? "app_list" : "notifications_list";
        assert(lv_obj_find_by_name(lv_screen_active(), list));
    }
    puts("PASS: left and right swipes still navigate to apps and notifications");

    home = new_home();
    touch(230, 160, true, 20);
    touch(230, 185, true, 20);
    touch(230, 160, true, 500);
    assert(lv_screen_active() == home);
    touch(230, 160, false, 20);
    puts("PASS: returning to the press origin does not re-enable long press");

    home = new_home();
    touch(230, 160, true, 20);
    touch(232, 162, true, 450);
    touch(232, 162, false, 20);
    /* Finish the screen transition. */
    settle();
    assert(lv_screen_active() != home);
    puts("PASS: stationary long press still opens watchface selection");
    return 0;
}
