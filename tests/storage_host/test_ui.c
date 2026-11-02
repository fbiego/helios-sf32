#include "ui_support.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "core/helios_storage_ui.c"
static cJSON *files[HELIOS_STORE_COUNT];
static bool preserve[HELIOS_STORE_COUNT];
static unsigned saves;
static bool save_fails;
bool helios_storage_ready(void) { return true; }
cJSON *helios_storage_document(void) {
    cJSON *d=cJSON_CreateObject();if(d && !cJSON_AddNumberToObject(d,"version",1)) {cJSON_Delete(d);return NULL;}return d;
}
cJSON *helios_storage_load(helios_store_id_t id, bool *exists) {
    *exists=files[id]!=NULL;return cJSON_Duplicate(files[id],true);
}
void helios_storage_preserve_invalid(helios_store_id_t id) { preserve[id]=true; }
bool helios_storage_save(helios_store_id_t id, const cJSON *doc) {
    if(save_fails || preserve[id]) return false;
    /* Exercise escaping and parsing rather than keeping pointers to snapshots. */
    char *text=cJSON_PrintUnformatted(doc);assert(text);
    cJSON *copy=cJSON_Parse(text);assert(copy);cJSON_free(text);
    cJSON_Delete(files[id]);files[id]=copy;saves++;return true;
}
void helios_prefs_retry_save(void) {}
static void *no_memory(size_t n) {(void)n;return NULL;}
static void profile_received(chronos_config_t config, uint32_t a, uint32_t b)
{
    if (config == CHRONOS_CONFIG_USER) helios_storage_ui_sync_user(a,b);
}
int main(void) {
    chronos_core_init();helios_notifications_init();helios_contacts_init();helios_weather_init();
    helios_storage_ui_init();helios_storage_ui_poll();
    assert(saves==5 && !helios_notifications_count() && !helios_contacts_count() && !helios_weather_hourly_count());
    helios_user_profile_t profile=helios_user_get();
    assert(profile.age==25 && profile.height==175 && profile.weight==70 && profile.step_length==70 && profile.target_steps==5000);
    assert(helios_user_calories(1000)==50 && helios_user_calories(0)==0);
    assert(helios_user_distance(1000,true)==0.7);
    /* Full BLE packet -> Chronos's packed callback -> profile -> JSON. */
    chronos_core_set_config_cb(profile_received);
    uint8_t payload[]={80,35,180,90,1,12,0}, packet[32];size_t length=0;
    assert(chronos_core_build_packet(0xAB,CHRONOS_PROTOCOL_STANDARD,0x74,0x80,payload,sizeof(payload),packet,sizeof(packet),&length)==CHRONOS_STATUS_OK);
    assert(chronos_core_receive_ble_packet(packet,length)==CHRONOS_STATUS_OK);
    profile=helios_user_get();assert(profile.age==35 && profile.height==180 && profile.weight==90 && profile.step_length==80 && profile.target_steps==12000);
    assert(helios_user_calories(1000)==74 && helios_user_calories(10000)==745);
    assert(helios_user_distance(1000,true)==0.8 && helios_user_distance(1000,false)>0.497 && helios_user_distance(1000,false)<0.498);
    assert(helios_notifications_add(icon_nt_whatsapp,"Quote \" and 雨","09:20","line\nslash\\"));
    const helios_notification_t *n=helios_notifications_get(0);assert(helios_notifications_set_read(n->id,true));
    chronos_core_t *core=chronos_core_get_state();core->contact_size=1;core->sos_contact=0;
    strcpy(core->contacts[0].name,"Alice");strcpy(core->contacts[0].number,"+254700000000");
    helios_storage_ui_sync_contacts();
    strcpy(core->qr_links[3],"https://example.com/a?x=\"b\"");helios_storage_ui_sync_qr(3);
    helios_subject_set_weather_location("Nairobi");helios_subject_set_weather_temp(25);
    assert(helios_weather_hourly_add(1,"10:00","25°","40%"));
    assert(helios_weather_daily_add(4,"Mon","20/27°"));
    unsigned before=saves;save_fails=true;helios_storage_ui_poll();assert(saves==before && dirty[HELIOS_STORE_NOTIFICATIONS]);
    save_fails=false;helios_storage_ui_poll();assert(saves==before+5);
    assert(num(files[HELIOS_STORE_USER],"age")==35 && num(files[HELIOS_STORE_USER],"height")==180 && num(files[HELIOS_STORE_USER],"weight")==90);
    assert(num(files[HELIOS_STORE_USER],"stepLength")==80 && num(files[HELIOS_STORE_USER],"targetSteps")==12000);
    assert(cJSON_IsNumber(item(cJSON_GetArrayItem(item(files[HELIOS_STORE_NOTIFICATIONS],"items"),0),"icon")));
    /* Simulate a reboot by restoring all collections, and verify settings are
     * not involved in list callbacks or notification read-state restore. */
    memset(qr,0,sizeof(qr));helios_storage_ui_init();
    profile=helios_user_get();assert(profile.weight==90 && profile.step_length==80 && profile.target_steps==12000 && helios_user_calories(1000)==74);
    assert(helios_notifications_count()==1);n=helios_notifications_get(0);
    assert(n->read && n->icon==icon_nt_whatsapp && !strcmp(n->message,"line\nslash\\") && !strcmp(n->title,"Quote \" and 雨"));
    assert(helios_contacts_count()==1 && !strcmp(helios_contacts_get(0)->name,"Alice") && sos_contact==0);
    assert(!strcmp(qr[3],"https://example.com/a?x=\"b\"") && !strcmp(core->qr_links[3],qr[3]));
    assert(helios_weather_hourly_count()==1 && helios_weather_daily_count()==1 && lv_subject_get_int(&sb_weather_temp)==25);
    assert(!strcmp(lv_subject_get_string(&sb_weather_location),"Nairobi"));
    before=saves;helios_storage_ui_poll();assert(saves==before); /* restore is not a change */
    helios_storage_ui_sync_user((35UL<<24)|(180UL<<16)|(90UL<<8)|80,12UL<<16);
    helios_storage_ui_poll();assert(saves==before); /* identical profile does not write */
    helios_storage_ui_sync_user((35UL<<24)|(180UL<<16)|80,12UL<<16); /* zero weight */
    assert(helios_user_get().weight==90 && !dirty[HELIOS_STORE_USER]);
    helios_user_profile_t extreme={255,255,255,255,UINT32_MAX};
    assert(helios_user_set(&extreme));
    assert(helios_user_calories(UINT32_MAX)==(uint32_t)((double)UINT32_MAX*255/100000.0*255*1.036));
    helios_storage_ui_sync_user((35UL<<24)|(180UL<<16)|(90UL<<8)|80,12UL<<16);
    helios_storage_ui_poll();
    helios_notifications_clear();helios_storage_ui_poll();helios_storage_ui_init();assert(!helios_notifications_count());
    helios_notifications_add(icon_nt_chat,"test","09:21","body");
    cJSON_Hooks hooks={no_memory,free};cJSON_InitHooks(&hooks);helios_storage_ui_poll();cJSON_InitHooks(NULL);
    assert(dirty[HELIOS_STORE_NOTIFICATIONS]);helios_storage_ui_poll();assert(!dirty[HELIOS_STORE_NOTIFICATIONS]);
    /* A malformed row must reject the complete document, before inserting any. */
    cJSON *array=cJSON_GetObjectItem(files[HELIOS_STORE_NOTIFICATIONS],"items");
    cJSON *bad=cJSON_CreateObject();cJSON_AddItemToArray(array,bad);cJSON_AddStringToObject(bad,"title","missing body");
    helios_storage_ui_init();assert(!helios_notifications_count() && preserve[HELIOS_STORE_NOTIFICATIONS]);
    cJSON_ReplaceItemInObject(files[HELIOS_STORE_USER],"stepLength",cJSON_CreateNumber(0));
    helios_storage_ui_init();assert(preserve[HELIOS_STORE_USER] && helios_user_get().step_length==70);
    puts("UI persistence: real collection APIs, observer batching, UTF-8/escaping, read flags, icon IDs, contacts/SOS, QR, weather, reboot/clear, OOM and invalid schema: passed");
    for(int i=0;i<HELIOS_STORE_COUNT;i++) cJSON_Delete(files[i]);
    return 0;
}
