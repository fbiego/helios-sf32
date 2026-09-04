/**
 * @file helios_ui.h
 */

#ifndef HELIOS_UI_H
#define HELIOS_UI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "helios_ui_gen.h"
#include "custom/apps/app_screens.h"
#include "custom/subjects/subjects.h"
#include "custom/events/events.h"
#include "custom/apps/weather/weather.h"
#include "custom/watchfaces/watchface_manager.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/
extern bool notifications_to_apps;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the component library
 */
void helios_ui_init(const char * asset_path);

void uart_log(const char *format, ...);

int32_t get_screen_width(int32_t index);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HELIOS_UI_H*/
