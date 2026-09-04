/**
 * @file app_init.h
 *
 */

#ifndef HELIOS_APP_INIT_H
#define HELIOS_APP_INIT_H
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /*********************
 *      INCLUDES
 *********************/
 #ifdef LV_LVGL_H_INCLUDE_SIMPLE
     #include "lvgl.h"
     #include "lvgl_private.h"
 #else
     #include "lvgl/lvgl.h"
     #include "lvgl/lvgl_private.h"
#endif
 
#include "app_manager.h"
#include "contacts/contacts.h"
#include "notifications/notifications.h"
#include "stopwatch/stopwatch.h"
#include "weather/weather.h"
#include "../watchfaces/watchface_manager.h"
 
 /*********************
 *      DEFINES
 *********************/
 
 /**********************
 *      TYPEDEFS
 **********************/
 
 /**********************
 * GLOBAL PROTOTYPES
 **********************/
void helios_apps_init_all(void);
 
 /**********************
 *      MACROS
 **********************/
 
 
 #ifdef __cplusplus
 } /*extern "C"*/
 #endif
 
#endif /*HELIOS_APP_INIT_H*/
