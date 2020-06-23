#ifndef __SETTINGSPRIVATE_H
#define __SETTINGSPRIVATE_H

#include "Basic/CommonMacro.h"
#include "lv_settings/lv_settings.h"
#include "GUI/DisplayPrivate.h"
#include "stdio.h"

void Creat_SettingsMain(lv_obj_t * parent);

void SettingsDisplay_EventHandler(lv_obj_t * obj, lv_event_t event);
void SettingsSound_EventHandler(lv_obj_t * obj, lv_event_t event);
void SettingsBattery_EventHandler(lv_obj_t * obj, lv_event_t event);
void SettingsRadio_EventHandler(lv_obj_t * obj, lv_event_t event);
void SettingsAbout_EventHandler(lv_obj_t * obj, lv_event_t event);

#endif
