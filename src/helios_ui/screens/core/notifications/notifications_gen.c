/**
 * @file notifications_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "notifications_gen.h"
#include "../../../helios_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * notifications_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_list;
    static lv_style_t style_cont;
    static lv_style_t style_cont_360;
    static lv_style_t style_cont_240;
    static lv_style_t style_pad_rect;
    static lv_style_t style_pad_rect_360;
    static lv_style_t style_pad_rect_240;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_list);
        lv_style_init(&style_cont);
        lv_style_init(&style_cont_360);
        lv_style_init(&style_cont_240);
        lv_style_init(&style_pad_rect);
        lv_style_init(&style_pad_rect_360);
        lv_style_init(&style_pad_rect_240);

        lv_style_set_bg_color(&style_list, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_list, 255);
        lv_style_set_pad_top(&style_cont, 120);
        lv_style_set_pad_bottom(&style_cont, 150);
        lv_style_set_pad_hor(&style_cont, 33);
        lv_style_set_flex_flow(&style_cont, LV_FLEX_FLOW_COLUMN_REVERSE);
        lv_style_set_pad_hor(&style_cont_360, 30);
        lv_style_set_pad_top(&style_cont_360, 80);
        lv_style_set_pad_bottom(&style_cont_360, 100);
        lv_style_set_pad_hor(&style_cont_240, 20);
        lv_style_set_pad_row(&style_cont_240, 5);
        lv_style_set_pad_top(&style_cont_240, 60);
        lv_style_set_pad_bottom(&style_cont_240, 80);
        lv_style_set_pad_hor(&style_pad_rect, 15);
        lv_style_set_pad_hor(&style_pad_rect_360, 13);
        lv_style_set_pad_hor(&style_pad_rect_240, 9);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HELIOS_UI_CHECK_COMPILE_TARGET(HELIOS_UI_TARGET_ALL)
    if (helios_ui_check_target(HELIOS_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "notifications_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * hs_text_small_0 = hs_text_small_create(lv_obj_0);
        lv_obj_set_width(hs_text_small_0, lv_pct(60));
        lv_label_set_text(hs_text_small_0, "No notifications avaialble, check back later");
        lv_label_set_translation_tag(hs_text_small_0, "no_notifications");
        lv_obj_set_style_text_align(hs_text_small_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_align(hs_text_small_0, LV_ALIGN_CENTER);

        lv_obj_t * notifications_list = wd_list_create(lv_obj_0);
        lv_obj_set_name(notifications_list, "notifications_list");
        wd_list_bind_screen(notifications_list, &sb_screen_size);
        lv_obj_t * wd_list_title_0 = wd_list_get_title(notifications_list);
        lv_obj_set_height(wd_list_title_0, LV_SIZE_CONTENT);
        lv_obj_t * hs_title_pill_0 = hs_title_pill_create(wd_list_title_0, "notifications");
        lv_obj_set_width(hs_title_pill_0, LV_SIZE_CONTENT);
        lv_obj_set_align(hs_title_pill_0, LV_ALIGN_CENTER);
        lv_obj_set_style_max_width(hs_title_pill_0, lv_pct(100), 0);

        lv_obj_t * wd_list_container_0 = wd_list_get_container(notifications_list);
        lv_obj_add_style(wd_list_container_0, &style_cont, 0);
        lv_obj_add_style(wd_list_container_0, &style_list, 0);
        lv_obj_bind_style(wd_list_container_0, &style_cont_360, 0, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_cont_240, 0, &sb_screen_size, 2);
        lv_obj_bind_state_if_eq(wd_list_container_0, &sb_screen_type, LV_STATE_USER_2, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect, LV_STATE_USER_2, &sb_screen_size, 0);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_360, LV_STATE_USER_2, &sb_screen_size, 1);
        lv_obj_bind_style(wd_list_container_0, &style_pad_rect_240, LV_STATE_USER_2, &sb_screen_size, 2);
        lv_obj_t * hs_button_0 = hs_button_create(wd_list_container_0, "Clear All", "clear_all", COLOR_DANGER);
        lv_obj_set_style_bg_color(hs_button_0, COLOR_BUTTON_DANGER, 0);
        lv_obj_set_style_radius(hs_button_0, 40, 0);

        notification_item_create(wd_list_container_0, icon_chat, "Alex", "14:02", "Hey Felix, just confirming if we’re still meeting at 4 PM at the usual spot. Let me know if there are any changes so I can plan accordingly.");

        notification_item_create(wd_list_container_0, icon_chat, "Gmail", "14:18", "Your verification code is 482913. This code will expire in 10 minutes. If you did not request this code, please secure your account immediately.");

        notification_item_create(wd_list_container_0, icon_timer, "Reminder", "15:00", "Project review meeting starts in 30 minutes. Please prepare the latest UI mockups and performance reports before joining the call.");

        notification_item_create(wd_list_container_0, icon_gear, "System", "15:12", "Battery level has dropped below 20%. To avoid interruptions, connect your device to a charger or enable power saving mode from settings.");

        notification_item_create(wd_list_container_0, icon_music, "Now playing", "15:27", "Midnight Drive - Synthwave Mix.\nEnjoy uninterrupted music streaming with premium quality audio and no ads.");

        notification_item_create(wd_list_container_0, icon_chat, "Twitter", "16:03", "You have 5 new followers and 2 new mentions. Tap to see who interacted with your latest post and join the conversation.");

        notification_item_create(wd_list_container_0, icon_running, "Chronos", "16:15", "Health summary update: Your average heart rate today is 78 bpm and you have completed 6,540 steps so far. Keep moving to reach your daily goal.");

        notification_item_create(wd_list_container_0, icon_chat, "Google Drive", "16:40", "Your file 'LVGL_UI_Mockup.fig' has been uploaded successfully and is now available across all your devices. Share it with collaborators to continue working.");

        notification_item_create(wd_list_container_0, icon_weather, "Weather", "17:05", "Light rain is expected around 6 PM with temperatures dropping to 18°C. Consider carrying an umbrella and wearing a light jacket.");

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

