#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "Module.h"
#include "TasksManage.h"
#include "wav_decoder.h"

String WavFilePath = "Music/UntAlg.wav";
extern WAV_TypeDef Wav_Handle;
#define WavProgress (((float)Wav_Handle.DataPosition/Wav_Handle.DataSize)*1000)

static lv_obj_t * labelWavInfo;

static lv_obj_t * barWav;
static lv_obj_t * labelWavTime;
static lv_task_t * taskWavBar;

static lv_obj_t * sliderWavVolume;
static lv_obj_t * labelWavVolume;

static lv_obj_t * btnWavCtrl;
static lv_obj_t * symWavCtrl;

static lv_obj_t * labelLrc;
static String LrcDisplay;

void Task_LabelLrcUpdate(const char* str)
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
    lv_label_set_text(labelLrc, LrcDisplay.c_str());
}

static void Creat_LabelLrc()
{
    labelLrc = lv_label_create(appWindow, NULL);
    lv_obj_align(labelLrc, sliderWavVolume, LV_ALIGN_OUT_BOTTOM_MID, -10, 10);
    lv_obj_set_auto_realign(labelLrc, true);
    
    LrcDisplay = "";
    lv_label_set_text_format(labelLrc, "<no xtrc file>");
}

static void Creat_WavFileInfo()
{
    labelWavInfo = lv_label_create(appWindow, NULL);
    lv_obj_align(labelWavInfo, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
    lv_obj_set_auto_realign(labelWavInfo, true);
    lv_label_set_text_format(
        labelWavInfo, 
        "Path:%s\n"
        "Size:%0.1fKB\n"
        "ChuckLength:%d\n"
        "EncodeType:%x\n"
        "ChannelCnt:%d\n"
        "SampleFreq:%d(%d)\n"
        "SampleBits:%d\n"
        "BytePerSecond:%d\n"
        "BytePerFrame:%d\n",
    
        WavFilePath.c_str(),
        ((Wav_Handle.Header.FileLength + 8)/1024.0f),
        Wav_Handle.Header.ChuckLength,
        Wav_Handle.Header.EncodeType,
        Wav_Handle.Header.ChannelCnt,
        Wav_Handle.Header.SampleFreq,Timer_GetClockOut(TIM2),
        Wav_Handle.Header.SampleBits,
        Wav_Handle.Header.BytePerSecond,
        Wav_Handle.Header.BytePerFrame
    );
}

static void Task_WavBarUpdate(lv_task_t * task)
{
    lv_bar_set_value(barWav, WavProgress, LV_ANIM_OFF);
    int sec = Wav_Handle.DataPosition / Wav_Handle.Header.BytePerSecond;
    int min = sec / 60;
    lv_label_set_text_format(labelWavTime, "%02d:%02d ", min, sec % 60);
    
    if(Wav_Handle.IsEnd)
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
    lv_label_set_text_format(labelWavTime, "00:00");
    
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
        
        lv_label_set_text_format(labelWavVolume, "%s%-3d", symGrp[index], volume);
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
        bool playing = WavPlayer_GetPlaying();
        lv_label_set_text(symWavCtrl, playing ? LV_SYMBOL_PLAY : LV_SYMBOL_PAUSE);
        WavPlayer_SetPlaying(!playing);
    }
}

void Creat_BtnWavCtrl()
{
    btnWavCtrl = lv_btn_create(appWindow, NULL);
    lv_obj_set_event_cb(btnWavCtrl, ButtonEvent_Handler);
    lv_obj_set_size(btnWavCtrl, 50, 50);
    lv_obj_align(btnWavCtrl, barWav, LV_ALIGN_OUT_TOP_MID, 0, -30);

    symWavCtrl = lv_label_create(btnWavCtrl, NULL);
    lv_label_set_text(symWavCtrl, LV_SYMBOL_PAUSE);

    lv_btn_set_ink_in_time(btnWavCtrl, 200);
    lv_btn_set_ink_out_time(btnWavCtrl, 200);
    
    lv_obj_set_protect(btnWavCtrl, LV_PROTECT_PRESS_LOST);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    if(WavPlayer_LoadFile(WavFilePath))
    {
        WavPlayer_SetEnable(true);
        WavPlayer_SetPlaying(true);
    }

    Creat_WavFileInfo();
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
    vTaskDelay(10);
    
    lv_obj_del_safe(&labelWavInfo);
    
    lv_obj_del_safe(&barWav);
    lv_obj_del_safe(&labelWavTime);
    
    lv_obj_del_safe(&sliderWavVolume);
    lv_obj_del_safe(&labelWavVolume);
    
    lv_obj_del_safe(&btnWavCtrl);
    lv_obj_del_safe(&labelLrc);
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
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
