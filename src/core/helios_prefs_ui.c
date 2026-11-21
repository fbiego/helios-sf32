#include "helios_prefs.h"
#include "helios_app.h"
#include "helios_features.h"
#include "helios_sounds.h"
#if HELIOS_HAS_BMI270
#include "helios_bmi270.h"
#endif

#include <rtthread.h>

#include "helios_ui/custom/apps/app_manager.h"
#include "helios_ui/custom/subjects/subjects.h"
#include "helios_ui/custom/watchfaces/watchface_manager.h"

#define LOG_TAG "helios.prefs.ui"
#include "log.h"

#define PREF_APP_LIST_MODE       "ui.app_list_mode"
#define PREF_SCREEN_BRIGHTNESS   "screen.brightness"
#define PREF_SCREEN_ROTATION     "screen.rotation"
#define PREF_SCREEN_RTW          "screen.raise_to_wake"
#define PREF_SCREEN_TIMEOUT      "screen.timeout"
#define PREF_FOCUSABLE           "notifications.focusable"
#define PREF_NOTIF_SOUND         "notifications.sound"
#define PREF_NOTIF_VIBRATION     "notifications.vibration"
#define PREF_GESTURE_NAV         "ui.gesture_navigation"
#define PREF_WRIST_POSITION      "ui.wrist_position"
#define PREF_LANGUAGE            "ui.language"
#define PREF_WATCHFACE_TAG       "ui.watchface.tag"

static bool g_restoring;

static void helios_prefs_ui_early_restore(void);

#if defined(__GNUC__) || defined(__clang__)
static void __attribute__((constructor)) helios_prefs_ui_register_initializer(void)
#else
static void helios_prefs_ui_register_initializer(void)
#endif
{
    helios_apps_initializer_add(helios_prefs_ui_early_restore);
}

static int32_t clamp_i32(int32_t value, int32_t min, int32_t max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

static void save_i32(const char *key, int32_t value)
{
    if (!g_restoring)
        helios_prefs_set_i32(key, value);
}

void helios_prefs_apply_saved_ui_settings(void)
{
    if (!helios_prefs_ready())
        return;

    int32_t value;
    char tag[HELIOS_WATCHFACE_TAG_MAX];

    g_restoring = true;

    if (helios_prefs_get_i32(PREF_APP_LIST_MODE, &value, 0))
        helios_subject_set_app_list_mode(clamp_i32(value, 0, 1));

    if (helios_prefs_get_i32(PREF_SCREEN_BRIGHTNESS, &value, 30))
        helios_subject_set_screen_brightness(clamp_i32(value, 1, 100));

    if (helios_prefs_get_i32(PREF_SCREEN_ROTATION, &value, 0))
        helios_subject_set_screen_rotation(clamp_i32(value, 0, 3));

    if (helios_prefs_get_i32(PREF_SCREEN_RTW, &value, 1))
        helios_subject_set_screen_rtw(clamp_i32(value, 0, 1));

    if (helios_prefs_get_i32(PREF_SCREEN_TIMEOUT, &value, 1))
        helios_subject_set_screen_timeout(clamp_i32(value, 0, 4));

    if (helios_prefs_get_i32(PREF_FOCUSABLE, &value, 1))
        helios_subject_set_focusable(clamp_i32(value, 0, 1));

    if (helios_prefs_get_i32(PREF_NOTIF_SOUND, &value, 0))
        helios_subject_set_notif_sound(value != 0);
    if (helios_prefs_get_i32(PREF_NOTIF_VIBRATION, &value, 0))
        helios_subject_set_notif_vibration(value != 0);
    if (helios_prefs_get_i32(PREF_GESTURE_NAV, &value, 0))
        helios_subject_set_gesture_navigation(value != 0);
    if (helios_prefs_get_i32(PREF_WRIST_POSITION, &value, 0))
        helios_subject_set_wrist_position(clamp_i32(value, 0, 1));

    if (helios_prefs_get_i32(PREF_LANGUAGE, &value, 0))
        helios_subject_set_language(value);

    if (helios_prefs_get_str(PREF_WATCHFACE_TAG, tag, sizeof(tag), "default")) {
        if (!helios_watchfaces_set_active_tag(tag))
            LOG_W("saved watchface not found: %s", tag);
    }

    g_restoring = false;
}

static void helios_prefs_ui_early_restore(void)
{
    helios_prefs_init();
    helios_prefs_apply_saved_ui_settings();
}

void helios_watchfaces_active_tag_changed(const char *tag)
{
    if (!g_restoring && tag && tag[0] != '\0')
        helios_prefs_set_str(PREF_WATCHFACE_TAG, tag);
}

void helios_subject_app_list_mode_change(int32_t value)
{
    save_i32(PREF_APP_LIST_MODE, clamp_i32(value, 0, 1));
}

void helios_subject_screen_rotation_change(int32_t value)
{
    save_i32(PREF_SCREEN_ROTATION, clamp_i32(value, 0, 3));
}

void helios_subject_screen_rtw_change(int32_t value)
{
    save_i32(PREF_SCREEN_RTW, clamp_i32(value, 0, 1));
}

void helios_subject_focusable_change(int32_t value)
{
    save_i32(PREF_FOCUSABLE, clamp_i32(value, 0, 1));
}

void helios_subject_notif_sound_change(int32_t value)
{
    helios_sounds_set_notification_sound(value != 0);
    save_i32(PREF_NOTIF_SOUND, value != 0);
}

void helios_subject_notif_vibration_change(int32_t value)
{
    helios_sounds_set_notification_vibration(value != 0);
    save_i32(PREF_NOTIF_VIBRATION, value != 0);
}

void helios_subject_gesture_navigation_change(int32_t value)
{
#if HELIOS_HAS_BMI270 && HELIOS_IMU_WRIST_GESTURES
    helios_bmi270_set_gesture_polling(value != 0 && helios_screen_is_on());
#endif
    save_i32(PREF_GESTURE_NAV, value != 0);
}

void helios_subject_wrist_position_change(int32_t value)
{
    value = clamp_i32(value, 0, 1);
#if HELIOS_HAS_BMI270 && HELIOS_IMU_WRIST_GESTURES
    helios_bmi270_set_wearable_arm((uint8_t)value);
#endif
    save_i32(PREF_WRIST_POSITION, value);
}

void helios_subject_language_change(int32_t value)
{
    save_i32(PREF_LANGUAGE, value);
}

void helios_prefs_save_screen_brightness(int32_t value)
{
    save_i32(PREF_SCREEN_BRIGHTNESS, clamp_i32(value, 1, 100));
}

void helios_prefs_save_screen_timeout(int32_t value)
{
    save_i32(PREF_SCREEN_TIMEOUT, clamp_i32(value, 0, 4));
}
