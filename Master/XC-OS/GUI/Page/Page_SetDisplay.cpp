#include "FileGroup.h"
#include "DisplayPrivate.h"

static lv_obj_t * appWindow;

/********************* Theme Ctrl ***********************/
//static lv_theme_t * th_act;

//static const char * th_options =
//{

//#if LV_USE_THEME_NIGHT
//        "Night"
//#endif

//#if LV_USE_THEME_MATERIAL
//        "\nMaterial"
//#endif

//#if LV_USE_THEME_ALIEN
//        "\nAlien"
//#endif

//#if LV_USE_THEME_ZEN
//        "\nZen"
//#endif

//#if LV_USE_THEME_NEMO
//        "\nNemo"
//#endif

//#if LV_USE_THEME_MONO
//        "\nMono"
//#endif

//#if LV_USE_THEME_DEFAULT
//        "\nDefault"
//#endif
//        ""
//};

//static lv_theme_t * themes[8];

//static void init_all_themes(uint16_t hue)
//{
//    /* NOTE: This must be adjusted if more themes are added. */
//    int i = 0;
//#if LV_USE_THEME_NIGHT
//    themes[i++] = lv_theme_night_init(hue, NULL);
//#endif

//#if LV_USE_THEME_MATERIAL
//    themes[i++] = lv_theme_material_init(hue, NULL);
//#endif

//#if LV_USE_THEME_ALIEN
//    themes[i++] = lv_theme_alien_init(hue, NULL);
//#endif

//#if LV_USE_THEME_ZEN
//    themes[i++] = lv_theme_zen_init(hue, NULL);
//#endif

//#if LV_USE_THEME_NEMO
//    themes[i++] = lv_theme_nemo_init(hue, NULL);
//#endif

//#if LV_USE_THEME_MONO
//    themes[i++] = lv_theme_mono_init(hue, NULL);
//#endif

//#if LV_USE_THEME_DEFAULT
//    themes[i++] = lv_theme_default_init(hue, NULL);
//#endif
//}

//static void hue_select_event_cb(lv_obj_t * roller, lv_event_t event)
//{

//    if(event == LV_EVENT_VALUE_CHANGED) {
//        uint16_t hue = lv_roller_get_selected(roller) * 30;

//        init_all_themes(hue);

//        lv_theme_set_current(th_act);
//    }
//}
static lv_obj_t * colorPicker;
static lv_obj_t * labelHue;
static uint16_t theme_hue_now = 200;

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
        lv_theme_set_current(lv_theme_material_init(theme_hue_now, LV_FONT_DEFAULT));
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
    lv_obj_align(colorPicker, NULL, LV_ALIGN_CENTER, 0, 0);
    /* Set the styles */
    lv_cpicker_set_style(colorPicker, LV_CPICKER_STYLE_MAIN, &styleMain);
    lv_cpicker_set_style(colorPicker, LV_CPICKER_STYLE_INDICATOR, &styleIndicator);
    /* Change the knob's color to that of the selected color */
    lv_cpicker_set_indic_colored(colorPicker, true);

    lv_cpicker_set_hue(colorPicker, theme_hue_now);
    lv_obj_set_event_cb(colorPicker, ColorPicker_EventHandler);
}


/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    lv_obj_move_foreground(appWindow);
    Creat_ColorPicker();

    labelHue = lv_label_create(colorPicker, NULL);
    lv_obj_set_auto_realign(labelHue, true);
    lv_obj_align(labelHue, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_color_t color = lv_cpicker_get_color(colorPicker);
    lv_label_set_text_fmt(labelHue, "R%d G%d B%d", color.ch.red << 3, color.ch.green << 2, color.ch.blue << 3);
}

/**
  * @brief  ҳ��ѭ���¼�
  * @param  ��
  * @retval ��
  */
static void Loop()
{
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    lv_obj_clean(appWindow);
}

/**
  * @brief  ҳ���¼�
  * @param  event:�¼����
  * @param  param:�¼�����
  * @retval ��
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
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_SetDisplay(uint8_t pageID)
{
    appWindow = AppWindow_GetObj(pageID);
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
