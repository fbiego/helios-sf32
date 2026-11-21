#include "helios_notification_ui.h"

#include "helios_ui/helios_ui.h"
#include "helios_ui/custom/apps/app_screens.h"
#include "helios_ui/custom/apps/notifications/notifications.h"

#define HELIOS_NOTIFICATION_PREVIEW_MS 3000U

static lv_obj_t *preview_screen;
static uint32_t preview_shown_at;
static bool preview_interacted;
static uint32_t preview_ids[HELIOS_NOTIFICATIONS_MAX];
static uint32_t preview_id_count;
static uint32_t preview_id_next;

void helios_notification_preview_reset(void)
{
    preview_id_count = 0;
    preview_id_next = 0;
}

void helios_notification_preview_add(uint32_t notification_id)
{
    preview_ids[preview_id_next] = notification_id;
    preview_id_next = (preview_id_next + 1) % HELIOS_NOTIFICATIONS_MAX;
    if (preview_id_count < HELIOS_NOTIFICATIONS_MAX)
        ++preview_id_count;
}

static const helios_notification_t *newest_unread(void)
{
    if (!preview_id_count)
        return NULL;

    for (uint32_t index = helios_notifications_count(); index > 0; --index) {
        const helios_notification_t *notification = helios_notifications_get(index - 1);
        if (!notification || notification->read)
            continue;
        for (uint32_t i = 0; i < preview_id_count; ++i) {
            if (preview_ids[i] == notification->id)
                return notification;
        }
    }
    return NULL;
}

static bool preview_render(lv_obj_t *dialog, const helios_notification_t *notification)
{
    lv_obj_t *icon = lv_obj_find_by_name(dialog, "icon");
    lv_obj_t *title = lv_obj_find_by_name(dialog, "title");
    lv_obj_t *message = lv_obj_find_by_name(dialog, "message");
    lv_obj_t *time_label = lv_obj_find_by_name(dialog, "time");
    if (!icon || !title || !message || !time_label)
        return false;

    lv_image_set_src(icon, notification->icon ? notification->icon : icon_nt_chat);
    lv_label_set_text(title, notification->title);
    lv_label_set_text(message, notification->message);
    lv_label_set_text(time_label, notification->time);

    lv_obj_t *message_container = lv_obj_get_parent(message);
    lv_obj_set_scroll_dir(dialog, LV_DIR_VER);
    lv_obj_set_scroll_dir(message_container, LV_DIR_VER);
    lv_obj_scroll_to_y(message_container, 0, LV_ANIM_OFF);
    lv_obj_scroll_to_y(dialog, 0, LV_ANIM_OFF);
    return true;
}

bool helios_notification_preview_is_open(void)
{
    return preview_screen && lv_screen_active() == preview_screen;
}

void helios_notification_preview_interact(void)
{
    if (helios_notification_preview_is_open())
        preview_interacted = true;
}

bool helios_notification_preview_poll(void)
{
    if (!helios_notification_preview_is_open() || preview_interacted)
        return false;

    if (lv_tick_elaps(preview_shown_at) < HELIOS_NOTIFICATION_PREVIEW_MS)
        return true;

    helios_notification_preview_dismiss();
    return false;
}

void helios_notification_preview_dismiss(void)
{
    if (helios_notification_preview_is_open())
        helios_screen_load_transition(screen_home(), LV_SCR_LOAD_ANIM_NONE, 0, true,
                                      HELIOS_SCREEN_TRANSITION_DEFAULT);
}

void helios_notification_preview_next(void)
{
    if (!helios_notification_preview_is_open())
        return;

    const helios_notification_t *notification = newest_unread();
    if (!notification) {
        helios_notification_preview_dismiss();
        return;
    }

    if (preview_render(lv_obj_get_child(preview_screen, 0), notification)) {
        preview_shown_at = lv_tick_get();
        helios_notifications_set_read(notification->id, true);
    }
}

static void preview_event(lv_event_t *event)
{
    if (lv_event_get_target(event) != preview_screen)
        return;

    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_DELETE) {
        preview_screen = NULL;
    } else if (code == LV_EVENT_GESTURE) {
        lv_indev_t *indev = lv_indev_active();
        if (!indev)
            return;

        lv_dir_t direction = lv_indev_get_gesture_dir(indev);
        if (direction == LV_DIR_LEFT || direction == LV_DIR_RIGHT) {
            lv_indev_wait_release(indev);
            if (direction == LV_DIR_LEFT)
                helios_notification_preview_next();
            else
                helios_notification_preview_dismiss();
        }
    }
}

bool helios_notification_preview_show(void)
{
    const helios_notification_t *notification = newest_unread();
    if (!notification || preview_screen)
        return false;

    lv_obj_t *screen = lv_obj_create(NULL);
    if (!screen)
        return false;

    lv_obj_remove_style_all(screen);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *dialog = notification_dialog_create(screen);
    if (!dialog || !preview_render(dialog, notification)) {
        lv_obj_delete(screen);
        return false;
    }

    preview_screen = screen;
    lv_obj_add_event_cb(screen, preview_event, LV_EVENT_ALL, NULL);
    helios_screen_load_transition(screen, LV_SCR_LOAD_ANIM_NONE, 0, true,
                                  HELIOS_SCREEN_TRANSITION_DEFAULT);
    preview_shown_at = lv_tick_get();
    preview_interacted = false;
    lv_display_trigger_activity(NULL);
    helios_notifications_set_read(notification->id, true);
    return true;
}

void helios_notification_list_mark_visible_read(void)
{
    if (lv_display_get_screen_loading(NULL))
        return;

    lv_obj_t *screen = lv_screen_active();
    const char *name = screen ? lv_obj_get_name(screen) : NULL;
    if (!name || !lv_streq(name, "notifications_#"))
        return;

    lv_obj_t *list = get_list_from_wd(screen, "notifications_list");
    if (!list || lv_obj_is_scrolling(list))
        return;

    /* The Notifications screen renders one named row per record, in store
     * order. Collect IDs first because marking a row read rebuilds the list. */
    uint32_t visible[HELIOS_NOTIFICATIONS_MAX];
    uint32_t count = 0;
    uint32_t index = 0;
    for (uint32_t i = 0; i < lv_obj_get_child_count(list); ++i) {
        lv_obj_t *row = lv_obj_get_child(list, (int32_t)i);
        const char *row_name = lv_obj_get_name(row);
        if (!row_name || !lv_streq(row_name, "notification_item_#"))
            continue;

        const helios_notification_t *notification = helios_notifications_get(index++);
        if (notification && !notification->read && lv_obj_is_visible(row))
            visible[count++] = notification->id;
    }
    for (uint32_t i = 0; i < count; ++i)
        helios_notifications_set_read(visible[i], true);
}
