#ifndef HELIOS_WEATHER_UI_H
#define HELIOS_WEATHER_UI_H
#include <time.h>
/* UI thread only. Render on receipt; poll handles boot and calendar-hour changes. */
void helios_weather_render(time_t now);
void helios_weather_poll(time_t now);
#endif
