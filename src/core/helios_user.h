#ifndef HELIOS_USER_H
#define HELIOS_USER_H
#include <stdint.h>
#include <stdbool.h>
#include "cJSON.h"

typedef struct {
    uint8_t age;        /* years */
    uint8_t height;     /* cm */
    uint8_t weight;     /* kg */
    uint8_t step_length;/* cm */
    uint32_t target_steps;
} helios_user_profile_t;

/* Normal thread context. Profile snapshots are safe across UI/storage threads. */
void helios_user_reset_defaults(void);
helios_user_profile_t helios_user_get(void);
bool helios_user_set(const helios_user_profile_t *profile);
bool helios_user_decode(uint32_t a, uint32_t b, helios_user_profile_t *profile);
bool helios_user_restore(const cJSON *document);
bool helios_user_serialize(cJSON *document);
uint32_t helios_user_calories(uint32_t steps);
double helios_user_distance(uint32_t steps, bool metric);
#endif
