#include "SettingsPrivate.h"

void SettingsBattery_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    page.PagePush(PAGE_BattInfo);
}
