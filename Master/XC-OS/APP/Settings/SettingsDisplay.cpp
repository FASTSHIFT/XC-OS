#include "SettingsPrivate.h"
#include "GUI/DisplayPrivate.h"
#include "BSP/BSP.h"

static void Backlight_Event(lv_settings_item_t * act_item, lv_event_t event)
{
    if(event == LV_EVENT_REFRESH)
    {
        static char valueStr[10];
        int state = map(Backlight_GetValue(), 0, 1000, 0, 256);
        act_item->state = state;
        act_item->value = valueStr;
        sprintf((char*)act_item->value, "%d%%", Backlight_GetValue() / 10);
    }
    else if(event == LV_EVENT_VALUE_CHANGED)
    {
        int value = map(act_item->state, 0, 256, 5, 1000);
        sprintf((char*)act_item->value, "%d%%", value / 10);
        Backlight_SetGradual(value);
    }
}

static uint16_t theme_hue_current = 200;
static lv_theme_t * theme_current;
static const char * th_options =
{
    
#if LV_USE_THEME_MATERIAL
        "Material"
#endif
    
#if LV_USE_THEME_NIGHT
        "\nNight"
#endif

#if LV_USE_THEME_ALIEN
        "\nAlien"
#endif

#if LV_USE_THEME_ZEN
        "\nZen"
#endif

#if LV_USE_THEME_NEMO
        "\nNemo"
#endif

#if LV_USE_THEME_MONO
        "\nMono"
#endif

#if LV_USE_THEME_DEFAULT
        "\nDefault"
#endif
};

static lv_theme_t * themes[8];

static void init_all_themes(uint16_t hue)
{
    /* NOTE: This must be adjusted if more themes are added. */
    int i = 0;

#if LV_USE_THEME_MATERIAL
    themes[i++] = lv_theme_material_init(hue, NULL);
#endif
    
#if LV_USE_THEME_NIGHT
    themes[i++] = lv_theme_night_init(hue, NULL);
#endif

#if LV_USE_THEME_ALIEN
    themes[i++] = lv_theme_alien_init(hue, NULL);
#endif

#if LV_USE_THEME_ZEN
    themes[i++] = lv_theme_zen_init(hue, NULL);
#endif

#if LV_USE_THEME_NEMO
    themes[i++] = lv_theme_nemo_init(hue, NULL);
#endif

#if LV_USE_THEME_MONO
    themes[i++] = lv_theme_mono_init(hue, NULL);
#endif

#if LV_USE_THEME_DEFAULT
    themes[i++] = lv_theme_default_init(hue, NULL);
#endif
}

static void Color_Event(lv_settings_item_t * act_item, lv_event_t event)
{
    if(event == LV_EVENT_REFRESH)
    {
        theme_current = lv_theme_get_current();
        static char valueStr[20];
        int state = map(theme_hue_current, 0, 360, 0, 256);
        act_item->state = state;
        act_item->value = valueStr;
        sprintf((char*)act_item->value, "Hue: %d", theme_hue_current);
    }
    else if(event == LV_EVENT_VALUE_CHANGED)
    {
        theme_hue_current = map(act_item->state, 0, 256, 0, 360);
        sprintf((char*)act_item->value, "Hue: %d", theme_hue_current);
        init_all_themes(theme_hue_current);
        lv_theme_set_current(theme_current);
    }
}

static void Theme_Event(lv_settings_item_t * act_item, lv_event_t event)
{
    if(event == LV_EVENT_REFRESH)
    {
        theme_current = lv_theme_get_current();
        act_item->value = th_options;
        act_item->state = 0;
    }
    else if(event == LV_EVENT_VALUE_CHANGED)
    {
        int opt = act_item->state;
        if(opt < __Sizeof(themes) && themes[opt] != NULL)
        {
            theme_current = themes[opt];
            init_all_themes(theme_hue_current);
            lv_theme_set_current(theme_current);
        }
    }
}

typedef void(*item_func_callback_t)(lv_settings_item_t*, lv_event_t);
#define ITEM_DEF(item_type,item_name) {.type = item_type, .name = #item_name, .user_data.ptr = (void*)item_name##_Event}

static lv_settings_item_t items[] =
{
    ITEM_DEF(LV_SETTINGS_TYPE_SLIDER, Backlight),
    ITEM_DEF(LV_SETTINGS_TYPE_SLIDER, Color),
    ITEM_DEF(LV_SETTINGS_TYPE_DDLIST, Theme)
};

void SettingsDisplay_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    lv_settings_item_t * act_item = (lv_settings_item_t *)lv_event_get_data();

    for(int i = 0; i < __Sizeof(items); i++)
    {
        /*初始化*/
        if(event == LV_EVENT_REFRESH)
        {
            item_func_callback_t func = (item_func_callback_t)items[i].user_data.ptr;
            if(func)
            {
                func(&items[i], event);
                lv_settings_add(&items[i]);
            }
        }
        /*值变化*/
        else if(event == LV_EVENT_VALUE_CHANGED)
        {
            if(strcmp(items[i].name, act_item->name) == 0)
            {
                item_func_callback_t func = (item_func_callback_t)(act_item->user_data.ptr);
                if(func)
                {
                    func(act_item, event);
                    if(act_item->type != LV_SETTINGS_TYPE_DDLIST)
                    {
                        lv_settings_refr(act_item);
                    }
                }
            }
        }
    }
}
