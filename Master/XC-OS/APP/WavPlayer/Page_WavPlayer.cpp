#include "DisplayPrivate.h"
#include "WavPlayer_Private.h"

/*页面容器*/
static lv_obj_t * appWindow;

/*信息*/
static lv_obj_t * labelWavInfo;

/*频谱*/
static lv_obj_t * chartFFT;
static lv_chart_series_t * serFFT;
static lv_task_t * taskSerFFT;

/*歌词*/
static lv_obj_t * labelLrc;

/*进度*/
static lv_obj_t * barWav;
static lv_obj_t * labelWavTime;
static lv_obj_t * labelWavTotalTime;
static lv_task_t * taskWavBar;

/*音量*/
static lv_obj_t * sliderWavVolume;

/*控制按钮*/
typedef struct
{
    const char * sym;
    lv_coord_t x_offset;
    lv_obj_t * btn;
    lv_obj_t * label;
} btnGrp_TypeDef;
/*按钮编号*/
enum btnWavID
{
    BTN_PREV,//上一曲
    BTN_STOP,//播放-暂停
    BTN_NEXT,//下一曲
};
static btnGrp_TypeDef btnWav_Grp[] =
{
    {LV_SYMBOL_PREV, -80},
    {LV_SYMBOL_PAUSE, 0},
    {LV_SYMBOL_NEXT, 80},
};

enum{
    BTN_MODE,
    BTN_LIST,
    BTN_VOLUME,
    BTN_INFO
};
static btnGrp_TypeDef btnListCtrl_Grp[] =
{
    {LV_SYMBOL_LOOP, -120},
    {LV_SYMBOL_LIST, -40},
    {LV_SYMBOL_VOLUME_MID, 40},
    {LV_SYMBOL_AUDIO, 120}
};

/*WavParam*/
static String WavFilePath;
static bool WavError;

static void Wav_Update();

void Wav_OpenFile(const char* path)
{
    WavFilePath = String(path);
    page.PagePush(PAGE_WavPlayer);
}

static void Creat_WavFileInfo()
{
    labelWavInfo = lv_label_create(appWindow, NULL);
    lv_label_set_static_text(labelWavInfo, WavFilePath.c_str());
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

    taskSerFFT = lv_task_create(Task_UpdateSerFFT, 50, LV_TASK_PRIO_LOW, 0);

    static lv_anim_t a;
    lv_obj_add_anim(
        chartFFT, &a,
        (lv_anim_exec_xcb_t)lv_obj_set_height,
        lv_obj_get_height(chartFFT) / 10, lv_obj_get_height(chartFFT)
    );
}

static String LrcDisplay;
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
    LrcDisplay = "";
    lv_label_set_static_text(labelLrc, LrcDisplay.c_str());
    
//    static lv_style_t style = *lv_label_get_style(labelLrc, LV_LABEL_STYLE_MAIN);
//    style.text.font = &lv_font_roboto_22;
//    lv_label_set_style(labelLrc, LV_LABEL_STYLE_MAIN, &style);
    lv_obj_align(labelLrc, barWav, LV_ALIGN_OUT_BOTTOM_MID, 0, -60);
    
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
    {
        MusicList_PlayNext(true);//page.PagePop();
        WavFilePath = MusicList_GetNowMusic();
        Wav_Update();
    }
}

static void Creat_WavBar()
{
    barWav = lv_bar_create(appWindow, NULL);
    lv_obj_set_size(barWav, APP_WIN_WIDTH - 100, 10);
    lv_obj_align(barWav, btnWav_Grp[BTN_STOP].btn, LV_ALIGN_OUT_TOP_MID, 0, -20);
    lv_bar_set_anim_time(barWav, 1000);
    lv_bar_set_range(barWav, 0, 1000);

    labelWavTime = lv_label_create(appWindow, NULL);
    lv_label_set_text_fmt(labelWavTime, "00:00");
    lv_obj_align(labelWavTime, barWav, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    
    labelWavTotalTime = lv_label_create(appWindow, NULL);
    
    taskWavBar = lv_task_create(Task_WavBarUpdate, 500, LV_TASK_PRIO_MID, 0);
}

static void SliderEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        uint8_t volume = lv_slider_get_value(obj);
        WavPlayer_SetVolume(volume);
    }
}

static void Creat_VolumeSlider()
{
    sliderWavVolume = lv_slider_create(appWindow, NULL);
    //lv_obj_set_opa_scale_enable(sliderWavVolume true);
    lv_obj_set_hidden(sliderWavVolume, true);
    lv_obj_set_size(sliderWavVolume, 20, APP_WIN_HEIGHT / 3);
    lv_obj_align(sliderWavVolume, appWindow, LV_ALIGN_IN_RIGHT_MID, -20, 0);
    lv_obj_set_event_cb(sliderWavVolume, SliderEvent_Handler);
    lv_slider_set_range(sliderWavVolume, 0, 100);
    lv_slider_set_value(sliderWavVolume, WavPlayer_GetVolume(), LV_ANIM_ON);
}

static void ButtonEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        /******btnWav_Grp*****/
        if(obj == btnWav_Grp[BTN_STOP].btn)
        {
            bool isPlaying = WavPlayer_GetPlaying();
            lv_label_set_text(btnWav_Grp[BTN_STOP].label, isPlaying ? LV_SYMBOL_PLAY : LV_SYMBOL_PAUSE);
            WavPlayer_SetPlaying(!isPlaying);
        }
        if(obj == btnWav_Grp[BTN_NEXT].btn)
        {
            MusicList_PlayNext(true);
            WavFilePath = MusicList_GetNowMusic();
            Wav_Update();
        }
        if(obj == btnWav_Grp[BTN_PREV].btn)
        {
            MusicList_PlayNext(false);
            WavFilePath = MusicList_GetNowMusic();
            Wav_Update();
        }
        
        /******btnListCtrl_Grp*****/
        if(obj == btnListCtrl_Grp[BTN_MODE].btn)
        {
            const char* sym_mode[] = {LV_SYMBOL_LOOP, LV_SYMBOL_SHUFFLE, "1", LV_SYMBOL_REFRESH"1"};
            static uint8_t mode_index = 0;
            mode_index++;
            mode_index %= __Sizeof(sym_mode);
            btnListCtrl_Grp[BTN_MODE].sym = sym_mode[mode_index];
            lv_label_set_text(btnListCtrl_Grp[BTN_MODE].label, btnListCtrl_Grp[BTN_MODE].sym);
        }
        if(obj == btnListCtrl_Grp[BTN_LIST].btn)
        {
            MusicList_SetActivate(!MusicList_GetActivate());
        }
        if(obj == btnListCtrl_Grp[BTN_VOLUME].btn)
        {
            lv_obj_set_hidden(sliderWavVolume, !lv_obj_get_hidden(sliderWavVolume));
        }
    }
}

static void Creat_BtnGrp(btnGrp_TypeDef* btnGrp, uint16_t size, int16_t w, int16_t h, lv_coord_t y_mod)
{
    for(int i = 0; i < size; i++)
    {
        btnGrp[i].btn = lv_btn_create(appWindow, NULL);
        lv_obj_set_event_cb(btnGrp[i].btn, ButtonEvent_Handler);
        lv_obj_set_size(btnGrp[i].btn, w, h);
        lv_obj_align(btnGrp[i].btn, NULL, LV_ALIGN_IN_BOTTOM_MID, btnGrp[i].x_offset, y_mod);

        btnGrp[i].label = lv_label_create(btnGrp[i].btn, NULL);
        lv_label_set_text(btnGrp[i].label, btnGrp[i].sym);

        lv_btn_set_ink_in_time(btnGrp[i].btn, 200);
        lv_btn_set_ink_out_time(btnGrp[i].btn, 200);

        lv_obj_set_protect(btnGrp[i].btn, LV_PROTECT_PRESS_LOST);
    }
}

static void Wav_Update()
{
    lv_label_set_static_text(labelWavInfo, WavFilePath.c_str());
    
    LrcDisplay = "";
    lv_label_set_static_text(labelLrc, LrcDisplay.c_str());
    
    const WAV_TypeDef* handle = WavPlayer_GetWavHandle();
    int sec = handle->DataSize / handle->Header.BytePerSecond;
    int min = sec / 60;
    lv_label_set_text_fmt(labelWavTotalTime, "%02d:%02d", min, sec % 60);
    lv_obj_align(labelWavTotalTime, barWav, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    lv_obj_move_foreground(appWindow);
    
    Preloader_Activate(true, appWindow);
    
    MusicList_Init("//Music");

    if(WavFilePath == "")
    {
        WavFilePath = MusicList_GetNextMusic(true);
    }
    
    WavError = !WavPlayer_LoadFile(WavFilePath);
    if(!WavError)
    {
        WavPlayer_SetEnable(true);
    }

    Creat_WavFileInfo();
    Creat_ChartFFT();
    
    Creat_BtnGrp(btnWav_Grp, __Sizeof(btnWav_Grp), 50, 50, -40);
    Creat_BtnGrp(btnListCtrl_Grp, __Sizeof(btnListCtrl_Grp), 55, 20, -10);
    Creat_WavBar();
    Creat_LabelLrc();
    Creat_VolumeSlider();
    
    Wav_Update();
    
    Preloader_Activate(false, appWindow);
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
    appWindow = AppWindow_GetCont(pageID);
    lv_style_t * style = (lv_style_t *)lv_cont_get_style(appWindow, LV_CONT_STYLE_MAIN);
    *style = lv_style_pretty;
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
