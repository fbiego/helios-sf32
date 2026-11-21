#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chronos_core.h"
#include "core/helios_prefs.c"
#include "core/helios_time.c"

static char *stored;
static bool fail_save;
static int32_t shown_hour, shown_am;
static char shown_clock[16], shown_suffix[4];
int rt_mutex_init(struct rt_mutex *m, const char *n, int f) { (void)m;(void)n;(void)f;return 0; }
int rt_mutex_take(struct rt_mutex *m, int t) { (void)m;(void)t;return 0; }
int rt_mutex_release(struct rt_mutex *m) { (void)m;return 0; }
rt_base_t rt_hw_interrupt_disable(void) { return 0; }
void rt_hw_interrupt_enable(rt_base_t level) { (void)level; }
bool helios_storage_ready(void) { return true; }
cJSON *helios_storage_document(void)
{ cJSON *doc=cJSON_CreateObject();assert(cJSON_AddNumberToObject(doc,"version",1));return doc; }
cJSON *helios_storage_load(helios_store_id_t id, bool *exists)
{ assert(id==HELIOS_STORE_SETTINGS);*exists=stored!=NULL;return stored?cJSON_Parse(stored):NULL; }
void helios_storage_preserve_invalid(helios_store_id_t id) { (void)id;assert(0); }
bool helios_storage_save(helios_store_id_t id, const cJSON *doc)
{
    assert(id==HELIOS_STORE_SETTINGS);
    if(fail_save) return false;
    char *text=cJSON_PrintUnformatted(doc);if(!text)return false;
    free(stored);stored=text;return true;
}
void helios_subject_set_time_hour(int32_t value) { shown_hour=value; }
void helios_subject_set_time_am(int32_t value) { shown_am=value; }
void helios_subject_set_time_am_pm(const char *value) { snprintf(shown_suffix,sizeof(shown_suffix),"%s",value); }
void helios_subject_set_time_string(const char *value) { snprintf(shown_clock,sizeof(shown_clock),"%s",value); }

static void received(chronos_config_t config, uint32_t a, uint32_t b)
{ assert(config==CHRONOS_CONFIG_24_HOUR && a==0);helios_time_request_24_hour(b!=0); }
static void phone_mode(uint8_t value)
{
    uint8_t packet[16];size_t length=0;
    assert(chronos_core_build_packet(0xAB,CHRONOS_PROTOCOL_STANDARD,0x7C,0x80,
        &value,1,packet,sizeof(packet),&length)==CHRONOS_STATUS_OK);
    assert(chronos_core_receive_ble_packet(packet,length)==CHRONOS_STATUS_OK);
}
static void show(int hour)
{ struct tm calendar={0};calendar.tm_hour=hour;calendar.tm_min=7;helios_time_publish(&calendar); }
static bool persisted_24(void)
{
    cJSON *doc=cJSON_Parse(stored);
    const cJSON *value=cJSON_GetObjectItem(cJSON_GetObjectItem(doc,"values"),"time.hour_24");
    assert(cJSON_IsNumber(value));bool result=value->valueint!=0;cJSON_Delete(doc);return result;
}
static void restart(void)
{
    cJSON_Delete(settings);settings=values=NULL;initialized=false;
    hour_24=true; /* The module's power-on default. */
    assert(helios_prefs_init());helios_time_restore_format();
}
static void *no_memory(size_t size) { (void)size;return NULL; }

int main(void)
{
    chronos_core_init();chronos_core_set_config_cb(received);
    assert(helios_prefs_init());helios_time_restore_format();
    show(0);assert(shown_hour==0 && shown_am==-1 && !strcmp(shown_clock,"00:07") && !shown_suffix[0]);
    phone_mode(1);show(0);assert(!persisted_24());
    const struct {int raw,hour,am;const char *clock;} cases[]={
        {0,12,0,"12:07"},{1,1,0,"01:07"},{11,11,0,"11:07"},
        {12,12,1,"12:07"},{13,1,1,"01:07"},{23,11,1,"11:07"}
    };
    for(unsigned i=0;i<sizeof(cases)/sizeof(cases[0]);i++) {
        show(cases[i].raw);assert(shown_hour==cases[i].hour && shown_am==cases[i].am);
        assert(!strcmp(shown_clock,cases[i].clock) && !strcmp(shown_suffix,cases[i].am?"PM":"AM"));
    }
    restart();show(23);assert(shown_hour==11 && shown_am==1);
    phone_mode(0);show(23);assert(persisted_24() && shown_hour==23 && shown_am==-1 && !shown_suffix[0]);
    restart();show(0);assert(shown_hour==0 && shown_am==-1);

    /* Failed document queue writes retain the preference for retry. */
    fail_save=true;phone_mode(1);show(13);assert(shown_hour==1 && persisted_24());
    fail_save=false;helios_prefs_retry_save();assert(!persisted_24());
    phone_mode(0);show(13);assert(persisted_24());
    /* Allocation failure while setting the preference retries next refresh. */
    cJSON_Hooks hooks={no_memory,free};cJSON_InitHooks(&hooks);
    phone_mode(1);show(13);assert(shown_hour==1);
    cJSON_InitHooks(NULL);assert(persisted_24());show(13);assert(!persisted_24());
    phone_mode(1);phone_mode(0);show(13);assert(shown_hour==13 && shown_am==-1 && persisted_24());
    cJSON_Delete(settings);free(stored);
    puts("Clock mode: real phone packets, noon/midnight, AM/PM, persistence/restart, failed writes, OOM retry and latest update passed");
    return 0;
}
