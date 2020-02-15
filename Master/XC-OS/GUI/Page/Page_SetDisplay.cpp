#include "Basic/FileGroup.h"
#include "DisplayPrivate.h"

static lv_obj_t * appWindow;
static lv_obj_t * colorPicker;
static lv_obj_t * labelHue;
static lv_obj_t * rollerTheme;

static uint16_t theme_hue_now = 200;

/********************* Theme Ctrl ***********************/
static lv_theme_t * th_act;

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
        ""
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

static void RollerTheme_EventHandler(lv_obj_t * roller, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t opt = lv_roller_get_selected(roller);
        th_act = themes[opt];
        lv_theme_set_current(th_act);
    }
}

static void Creat_RollerTheme()
{
    rollerTheme = lv_roller_create(appWindow, NULL);
    lv_roller_set_options(rollerTheme, th_options, true);
    lv_obj_align(rollerTheme, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -50);
    lv_obj_set_event_cb(rollerTheme, RollerTheme_EventHandler);
}

static void ColorPicker_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    if(obj != colorPicker)
        return;
    
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        lv_color_t color = lv_cpicker_get_color(colorPicker);
        lv_label_set_text_fmt(labelHue, "R%d G%d B%d", color.ch.red << 3, color.ch.green << 2, color.ch.blue << 3);
    }
    else if(event == LV_EVENT_RELEASED)
    {
        theme_hue_now = lv_cpicker_get_hue(obj);
        init_all_themes(theme_hue_now);
        lv_theme_set_current(th_act);
    }
}

static void Creat_ColorPicker()
{
    const lv_coord_t pickerSize = 200;

    /* Set the style of the color ring */
    static lv_style_t styleMain;
    lv_style_copy(&styleMain, &lv_style_plain);
    styleMain.line.width = 30;
    /* Make the background white */
    styleMain.body.main_color = styleMain.body.grad_color = LV_COLOR_WHITE;

    /* Set the style of the knob */
    static lv_style_t styleIndicator;
    lv_style_copy(&styleIndicator, &lv_style_pretty);
    styleIndicator.body.border.color = LV_COLOR_WHITE;
    /* Ensure that the knob is fully opaque */
    styleIndicator.body.opa = LV_OPA_COVER;
    styleIndicator.body.border.opa = LV_OPA_COVER;

    colorPicker = lv_cpicker_create(appWindow, NULL);
    lv_obj_set_size(colorPicker, pickerSize, pickerSize);
    /* Choose the 'DISC' type */
    lv_cpicker_set_type(colorPicker, LV_CPICKER_TYPE_DISC);
    lv_obj_align(colorPicker, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
    /* Set the styles */
    lv_cpicker_set_style(colorPicker, LV_CPICKER_STYLE_MAIN, &styleMain);
    lv_cpicker_set_style(colorPicker, LV_CPICKER_STYLE_INDICATOR, &styleIndicator);
    /* Change the knob's color to that of the selected color */
    lv_cpicker_set_indic_colored(colorPicker, true);

    lv_cpicker_set_hue(colorPicker, theme_hue_now);
    lv_obj_set_event_cb(colorPicker, ColorPicker_EventHandler);
}

static void Creat_LabelHue()
{
    labelHue = lv_label_create(colorPicker, NULL);
    lv_obj_set_auto_realign(labelHue, true);
    lv_obj_align(labelHue, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_color_t color = lv_cpicker_get_color(colorPicker);
    lv_label_set_text_fmt(labelHue, "R%d G%d B%d", color.ch.red << 3, color.ch.green << 2, color.ch.blue << 3);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    lv_obj_move_foreground(appWindow);
    init_all_themes(theme_hue_now);
    Creat_ColorPicker();
    Creat_LabelHue();
    Creat_RollerTheme();
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_obj_clean(appWindow);
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        if(btn == btnBack)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_SetDisplay(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
