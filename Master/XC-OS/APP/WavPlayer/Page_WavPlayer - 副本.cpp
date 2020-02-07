#include "DisplayPrivate.h"
#include "WavPlayer_Private.h"

static lv_obj_t * appWindow;

/*WavParam*/
static String WavFilePath;
static bool WavError;

static lv_obj_t * labelWavInfo;

static lv_obj_t * chartFFT;
static lv_chart_series_t * serFFT;
static lv_task_t * taskSerFFT;

static lv_obj_t * barWav;
static lv_obj_t * labelWavTime;
static lv_obj_t * labelWavTotalTime;
static lv_task_t * taskWavBar;

static lv_obj_t * sliderWavVolume;
static lv_obj_t * labelWavVolume;


typedef struct
{
    lv_obj_t * btn;
    lv_obj_t * label;
    const char * sym;
    lv_coord_t x_offset;
} btnWavGrp_TypeDef;
enum btnWavID
{
    BTN_LAST,
    BTN_STOP,
    BTN_NEXT,
    BTN_MAX
};
static btnWavGrp_TypeDef btnWav_Grp[BTN_MAX] =
{
    {NULL, NULL, LV_SYMBOL_LEFT, -80},
    {NULL, NULL, LV_SYMBOL_PAUSE, 0},
    {NULL, NULL, LV_SYMBOL_RIGHT, 80},
};

static lv_obj_t * labelLrc;
static String LrcDisplay;

void Wav_OpenFile(const char* path)
{
    WavFilePath = String(path);
    page.PagePush(PAGE_WavPlayer);
}

void Wav_LabelLrcUpdate(const char* str)
{
    static bool readyBreak = false;

    if(readyBreak)
    {
        LrcDisplay = "";
        readyBreak = false;
    }

    String s = String(str);
    if(s.endsWith("\n"))
    {
        readyBreak = true;
    }
    LrcDisplay += s;

    lv_label_set_static_text(labelLrc, LrcDisplay.c_str());


    int lrc_width = lv_obj_get_width(labelLrc);
    int lrc_x =
        (lrc_width <= APP_WIN_WIDTH)
        ? ((APP_WIN_WIDTH - lrc_width) / 2)
        : (APP_WIN_WIDTH - lrc_width - 10);

    static lv_anim_t a;
    lv_obj_add_anim(
        labelLrc, &a,
        (lv_anim_exec_xcb_t)lv_obj_set_x,
        lv_obj_get_x(labelLrc), lrc_x,
        100
    );
}

static void Creat_LabelLrc()
{
    labelLrc = lv_label_create(appWindow, NULL);
    lv_obj_align(labelLrc, barWav, LV_ALIGN_OUT_BOTTOM_MID, 0, 60);

    LrcDisplay = "";
    lv_label_set_static_text(labelLrc, LrcDisplay.c_str());
}

static void Creat_WavFileInfo()
{
    labelWavInfo = lv_label_create(appWindow, NULL);
    lv_label_set_text_fmt(labelWavInfo, "File:%s", WavFilePath.c_str());
    lv_label_set_long_mode(labelWavInfo, LV_LABEL_LONG_SROLL);
    lv_label_set_anim_speed(labelWavInfo, 10);
    lv_obj_set_width(labelWavInfo, APP_WIN_WIDTH - 60);
    lv_obj_align(labelWavInfo, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
}

#define ChartPointCount 32

static void Task_UpdateSerFFT(lv_task_t * task)
{
    const float * fft_strength = FFT_GetStrength();
    for(int i = 0; i < ChartPointCount ; i++)
    {
        serFFT->points[i] = fft_strength[i * (128 / ChartPointCount) + 1];
    }
    lv_chart_refresh(chartFFT);
}

static void Creat_ChartFFT()
{
    chartFFT = lv_chart_create(appWindow, NULL);
    lv_obj_set_size(chartFFT, APP_WIN_WIDTH - 60, 120);
    lv_obj_align(chartFFT, labelWavInfo, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
    lv_obj_set_opa_scale_enable(chartFFT, true);
    lv_obj_set_opa_scale(chartFFT, LV_OPA_80);
    lv_chart_set_type(chartFFT, LV_CHART_TYPE_COLUMN);
    lv_chart_set_series_opa(chartFFT, LV_OPA_COVER);
    lv_chart_set_series_width(chartFFT, 10);
    lv_chart_set_update_mode(chartFFT, LV_CHART_UPDATE_MODE_CIRCULAR);

    lv_chart_set_range(chartFFT, 0, 1200);
    lv_chart_set_point_count(chartFFT, ChartPointCount);
    lv_chart_set_div_line_count(chartFFT, 0, 0);

    serFFT = lv_chart_add_series(chartFFT, lv_style_plain_color.body.main_color);

    taskSerFFT = lv_task_create(Task_UpdateSerFFT, 50, LV_TASK_PRIO_MID, 0);

    static lv_anim_t a;
    lv_obj_add_anim(
        chartFFT, &a,
        (lv_anim_exec_xcb_t)lv_obj_set_height,
        lv_obj_get_height(chartFFT) / 10, lv_obj_get_height(chartFFT)
    );
}

static void Task_WavBarUpdate(lv_task_t * task)
{
    const WAV_TypeDef* handle = WavPlayer_GetWavHandle();
#define WavProgress(handle) (((float)handle->DataPosition/handle->DataSize)*1000)
    lv_bar_set_value(barWav, WavProgress(handle), LV_ANIM_OFF);
    int sec = handle->DataPosition / handle->Header.BytePerSecond;
    int min = sec / 60;
    lv_label_set_text_fmt(labelWavTime, "%02d:%02d ", min, sec % 60);

    if(handle->IsEnd || WavError)
        page.PagePop();
}

static void Creat_WavBar()
{
    barWav = lv_bar_create(appWindow, NULL);
    lv_obj_set_size(barWav, APP_WIN_WIDTH - 30, 20);
    lv_obj_align(barWav, NULL, LV_ALIGN_CENTER, 0, 100);
    lv_bar_set_anim_time(barWav, 1000);
    lv_bar_set_range(barWav, 0, 1000);

    labelWavTime = lv_label_create(appWindow, NULL);
    lv_obj_align(labelWavTime, barWav, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_set_auto_realign(labelWavTime, true);
    lv_label_set_text_fmt(labelWavTime, "00:00");

    const WAV_TypeDef* handle = WavPlayer_GetWavHandle();
    int sec = handle->DataSize / handle->Header.BytePerSecond;
    int min = sec / 60;
    labelWavTotalTime = lv_label_create(appWindow, NULL);
    lv_label_set_text_fmt(labelWavTotalTime, "%02d:%02d", min, sec % 60);
    lv_obj_align(labelWavTotalTime, barWav, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);

    taskWavBar = lv_task_create(Task_WavBarUpdate, 500, LV_TASK_PRIO_MID, 0);
}

static void SliderEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        uint8_t volume = lv_slider_get_value(obj);
        WavPlayer_SetVolume(volume);
        const char* symGrp[] = {LV_SYMBOL_MUTE, LV_SYMBOL_VOLUME_MID, LV_SYMBOL_VOLUME_MAX};
        uint8_t index;
        if(volume < 20) index = 0;
        else if(volume < 60) index = 1;
        else index = 2;

        lv_label_set_text_fmt(labelWavVolume, "%s%-3d", symGrp[index], volume);
    }
}

static void Creat_VolumeSlider()
{
    sliderWavVolume = lv_slider_create(appWindow, NULL);
    lv_obj_set_size(sliderWavVolume, APP_WIN_WIDTH - 100, 10);
    lv_obj_align(sliderWavVolume, barWav, LV_ALIGN_OUT_BOTTOM_LEFT, 50, 30);
    lv_obj_set_event_cb(sliderWavVolume, SliderEvent_Handler);
    lv_slider_set_range(sliderWavVolume, 0, 100);
    lv_slider_set_value(sliderWavVolume, WavPlayer_GetVolume(), LV_ANIM_ON);

    labelWavVolume = lv_label_create(appWindow, NULL);
    lv_obj_align(labelWavVolume, sliderWavVolume, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    lv_obj_set_auto_realign(labelWavVolume, true);

    lv_event_send(sliderWavVolume, LV_EVENT_VALUE_CHANGED, 0);
}

static void ButtonEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        if(obj == btnWav_Grp[BTN_STOP].btn)
        {
            bool playing = WavPlayer_GetPlaying();
            lv_label_set_text(btnWav_Grp[BTN_STOP].label, playing ? LV_SYMBOL_PLAY : LV_SYMBOL_PAUSE);
            WavPlayer_SetPlaying(!playing);
        }
    }
}

void Creat_BtnWavCtrl()
{
#define BTN_SIZE 50
    for(int i = 0; i < BTN_MAX; i++)
    {
        btnWav_Grp[i].btn = lv_btn_create(appWindow, NULL);
        lv_obj_set_event_cb(btnWav_Grp[i].btn, ButtonEvent_Handler);
        lv_obj_set_size(btnWav_Grp[i].btn, BTN_SIZE, BTN_SIZE);
        lv_obj_align(btnWav_Grp[i].btn, barWav, LV_ALIGN_OUT_TOP_MID, btnWav_Grp[i].x_offset, -30);

        btnWav_Grp[i].label = lv_label_create(btnWav_Grp[i].btn, NULL);
        lv_label_set_text(btnWav_Grp[i].label, btnWav_Grp[i].sym);

        lv_btn_set_ink_in_time(btnWav_Grp[i].btn, 200);
        lv_btn_set_ink_out_time(btnWav_Grp[i].btn, 200);

        lv_obj_set_protect(btnWav_Grp[i].btn, LV_PROTECT_PRESS_LOST);
    }
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    lv_obj_move_foreground(appWindow);

    WavError = !WavPlayer_LoadFile(WavFilePath);
    if(!WavError)
    {
        WavPlayer_SetEnable(true);
        WavPlayer_SetPlaying(true);
    }

    Creat_WavFileInfo();
    Creat_ChartFFT();
    Creat_WavBar();
    Creat_VolumeSlider();
    Creat_BtnWavCtrl();
    Creat_LabelLrc();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    WavPlayer_SetEnable(false);
    lv_task_del(taskWavBar);
    lv_task_del(taskSerFFT);
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
void PageRegister_WavPlayer(uint8_t pageID)
{
    appWindow = Page_GetAppWindow(pageID);
    lv_style_t * style = (lv_style_t *)lv_cont_get_style(appWindow, LV_CONT_STYLE_MAIN);
    *style = lv_style_pretty;
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
