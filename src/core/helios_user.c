#include "helios_user.h"
#include <rtthread.h>

/* Match Chronos Watchy's HealthModule defaults and five profile fields. */
static const helios_user_profile_t defaults = {25, 175, 70, 70, 5000};
static helios_user_profile_t current = {25, 175, 70, 70, 5000};

void helios_user_reset_defaults(void)
{
    rt_enter_critical();
    current = defaults;
    rt_exit_critical();
}

helios_user_profile_t helios_user_get(void)
{
    rt_enter_critical();
    helios_user_profile_t profile = current;
    rt_exit_critical();
    return profile;
}

bool helios_user_set(const helios_user_profile_t *profile)
{
    /* Zero age/target can represent unspecified values; the dimensions used
     * for energy estimation must be positive. Never partially apply a packet. */
    if (!profile || !profile->height || !profile->weight || !profile->step_length) return false;
    rt_enter_critical();
    current = *profile;
    rt_exit_critical();
    return true;
}

bool helios_user_decode(uint32_t a, uint32_t b, helios_user_profile_t *profile)
{
    if (!profile) return false;
    helios_user_profile_t decoded = {
        .age = (uint8_t)(a >> 24), .height = (uint8_t)(a >> 16),
        .weight = (uint8_t)(a >> 8), .step_length = (uint8_t)a,
        .target_steps = ((b >> 16) & 0xffu) * 1000u
    };
    if (!decoded.height || !decoded.weight || !decoded.step_length) return false;
    *profile = decoded;
    return true;
}

static bool read_number(const cJSON *doc, const char *key, uint32_t max, uint32_t *out)
{
    const cJSON *v = cJSON_GetObjectItemCaseSensitive(doc, key);
    if (!cJSON_IsNumber(v) || v->valuedouble < 0 || v->valuedouble > max ||
        (double)(uint32_t)v->valuedouble != v->valuedouble) return false;
    *out = (uint32_t)v->valuedouble;
    return true;
}

bool helios_user_restore(const cJSON *doc)
{
    uint32_t age, height, weight, stride, target;
    if (!read_number(doc, "age", UINT8_MAX, &age) ||
        !read_number(doc, "height", UINT8_MAX, &height) ||
        !read_number(doc, "weight", UINT8_MAX, &weight) ||
        !read_number(doc, "stepLength", UINT8_MAX, &stride) ||
        !read_number(doc, "targetSteps", UINT32_MAX, &target)) return false;
    helios_user_profile_t profile = {(uint8_t)age, (uint8_t)height, (uint8_t)weight, (uint8_t)stride, target};
    return helios_user_set(&profile);
}

bool helios_user_serialize(cJSON *doc)
{
    helios_user_profile_t p = helios_user_get();
    return doc && cJSON_AddNumberToObject(doc, "age", p.age) &&
           cJSON_AddNumberToObject(doc, "height", p.height) &&
           cJSON_AddNumberToObject(doc, "weight", p.weight) &&
           cJSON_AddNumberToObject(doc, "stepLength", p.step_length) &&
           cJSON_AddNumberToObject(doc, "targetSteps", p.target_steps);
}

uint32_t helios_user_calories(uint32_t steps)
{
    helios_user_profile_t p = helios_user_get();
    /* Watchy: distance_km * weight_kg * 1.036, truncated to whole kcal.
     * Convert before multiplying to avoid uint32_t overflow in steps * stride. */
    double kcal = ((double)steps * p.step_length / 100000.0) * p.weight * 1.036;
    return kcal >= UINT32_MAX ? UINT32_MAX : (uint32_t)kcal;
}

double helios_user_distance(uint32_t steps, bool metric)
{
    helios_user_profile_t p = helios_user_get();
    double km = (double)steps * p.step_length / 100000.0;
    return metric ? km : km * 0.621371;
}
