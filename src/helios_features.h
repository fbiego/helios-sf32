#ifndef HELIOS_FEATURES_H
#define HELIOS_FEATURES_H

/* Board profiles own hardware capabilities. Unknown targets default to absent. */
#if defined(HELIOS_BOARD_SF32LB52_LCD_N16R8)
#include "boards/sf32lb52_lcd_n16r8.h"
#elif defined(HELIOS_BOARD_SF32LB52_LCD_CUSTOM_410X502)
#include "boards/sf32lb52_lcd_custom_410x502.h"
#elif defined(HELIOS_BOARD_SF32LB52_LCD_CUSTOM_466X466_CO5300)
#include "boards/sfwatch_v1_0.h"
#endif

#ifndef HELIOS_HAS_MAX30100
#define HELIOS_HAS_MAX30100 0
#endif
#ifndef HELIOS_HAS_BMI270
#define HELIOS_HAS_BMI270 0
#endif
#ifndef HELIOS_HAS_BUZZER
#define HELIOS_HAS_BUZZER 0
#endif
#ifndef HELIOS_HAS_MOTOR
#define HELIOS_HAS_MOTOR 0
#endif
#ifndef HELIOS_HAS_CHARGE_DETECT
#define HELIOS_HAS_CHARGE_DETECT 0
#endif
#ifndef HELIOS_CHARGE_DETECT_ACTIVE_LEVEL
#define HELIOS_CHARGE_DETECT_ACTIVE_LEVEL 0
#endif
#ifndef HELIOS_DISPLAY_ROTATION_CW
#define HELIOS_DISPLAY_ROTATION_CW 0
#endif
#ifndef HELIOS_HAS_LOW_POWER
#define HELIOS_HAS_LOW_POWER 0
#endif

/* IMU features can be selected independently in each board profile. */
#ifndef HELIOS_IMU_STEPS
#define HELIOS_IMU_STEPS HELIOS_HAS_BMI270
#endif
#ifndef HELIOS_IMU_RAISE_TO_WAKE
#define HELIOS_IMU_RAISE_TO_WAKE HELIOS_HAS_BMI270
#endif
#ifndef HELIOS_IMU_DEBUG_ORIENTATION
#define HELIOS_IMU_DEBUG_ORIENTATION HELIOS_HAS_BMI270
#endif
#ifndef HELIOS_IMU_DEBUG_GYRO
#define HELIOS_IMU_DEBUG_GYRO 0
#endif

/* Use #if, since disabled features are explicitly defined as 0. */
#ifndef HELIOS_APP_HEALTH
#define HELIOS_APP_HEALTH HELIOS_HAS_MAX30100
#endif
#ifndef HELIOS_APP_CONTACTS
#define HELIOS_APP_CONTACTS 1
#endif
#ifndef HELIOS_APP_FILES
#define HELIOS_APP_FILES 1
#endif
#ifndef HELIOS_APP_FITNESS
#define HELIOS_APP_FITNESS HELIOS_HAS_BMI270
#endif
#ifndef HELIOS_APP_MUSIC
#define HELIOS_APP_MUSIC 1
#endif
#ifndef HELIOS_APP_NAVIGATION
#define HELIOS_APP_NAVIGATION 1
#endif
#ifndef HELIOS_APP_MPESA
#define HELIOS_APP_MPESA 1
#endif
#ifndef HELIOS_APP_NOTIFICATIONS
#define HELIOS_APP_NOTIFICATIONS 1
#endif
#ifndef HELIOS_APP_PHONE_LINK
#define HELIOS_APP_PHONE_LINK 1
#endif
#ifndef HELIOS_APP_SETTINGS
#define HELIOS_APP_SETTINGS 1
#endif
#ifndef HELIOS_APP_SLEEP
#define HELIOS_APP_SLEEP 1
#endif
#ifndef HELIOS_APP_STOPWATCH
#define HELIOS_APP_STOPWATCH 1
#endif
#ifndef HELIOS_APP_TIMER
#define HELIOS_APP_TIMER 1
#endif
#ifndef HELIOS_APP_WEATHER
#define HELIOS_APP_WEATHER 1
#endif

#endif
