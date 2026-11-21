#ifndef HELIOS_STORAGE_H
#define HELIOS_STORAGE_H
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "cJSON.h"

typedef enum {
    HELIOS_STORE_SETTINGS, HELIOS_STORE_WEATHER, HELIOS_STORE_NOTIFICATIONS,
    HELIOS_STORE_CONTACTS, HELIOS_STORE_QR, HELIOS_STORE_USER, HELIOS_STORE_ALARMS,
    HELIOS_STORE_COUNT
} helios_store_id_t;

bool helios_storage_ready(void);
/* Thread context only: commit queued documents and current history before reboot.
 * Returns false on write failure; pending data remains available for retry. */
bool helios_storage_flush(void);
/* Boot-time reads only. Missing files return NULL with exists=false. Invalid
 * files are preserved and blocked from replacement, rather than overwritten. */
cJSON *helios_storage_load(helios_store_id_t id, bool *exists);
void helios_storage_preserve_invalid(helios_store_id_t id);
/* Copies JSON into a bounded, coalescing queue. No filesystem I/O in caller. */
bool helios_storage_save(helios_store_id_t id, const cJSON *document);
cJSON *helios_storage_document(void); /* New versioned object. */
void helios_storage_record_battery(time_t now, uint32_t mv, uint8_t percent, uint8_t charging);
void helios_storage_record_steps(time_t now, uint32_t raw_steps);
uint32_t helios_storage_daily_steps(void);
#define HELIOS_STEPS_HOURS_PER_DAY 24
/* Copy the worker's hourly snapshot for the local day of now, or zeros. */
void helios_storage_hourly_steps(time_t now, uint32_t bins[HELIOS_STEPS_HOURS_PER_DAY]);

typedef struct {
    uint32_t steps, calories;
    uint16_t year;
    uint8_t month, day, hour;
} helios_steps_record_t;
/* Called by the storage worker, outside storage locks; false stops the export. */
typedef bool (*helios_steps_record_cb_t)(const helios_steps_record_t *record, void *user);
/* Queue a paced export of retained hourly records, oldest first. NULL cancels.
 * No filesystem access in the caller; repeated requests replace the export. */
void helios_storage_request_steps(helios_steps_record_cb_t callback, void *user);
#endif
