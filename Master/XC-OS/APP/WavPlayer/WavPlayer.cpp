#include "GUI/DisplayPrivate.h"
#include "Basic/TasksManage.h"
#include "FifoQueue/FifoQueue.h"
#include "WavPlayer_Private.h"

/*WAV缓冲队列*/
#define WAV_BUFF_SIZE (8 * 1024)
static uint8_t waveBuff[WAV_BUFF_SIZE];
static FifoQueue<uint8_t> WaveFifo(waveBuff, WAV_BUFF_SIZE);

/*WAV文件信息*/
static File WavFile;
static WAV_TypeDef Wav_Handle;

/*WAV播放控制*/
#define WAV_VOLUME_MAX 0.7f
static float Wav_Volume = WAV_VOLUME_MAX / 5;
static bool Wav_Playing = true;

/*WAV播放任务句柄*/
TaskHandle_t TaskHandle_WavPlayer;

static void WavTimer_Handler()
{
    uint32_t WavAvailable = WaveFifo.available();
    if(WavAvailable < 16)
    {
        return;
    }
    else
    {
        uint16_t data = ((Wav_Handle.CurrentData.LeftVal + 32768) >> 6) * Wav_Volume;
        Wav_Next_16Bit2Channel(&Wav_Handle);
        Audio_WriteData(data);
        FFT_AddData(data);
    }
}

static uint8_t WavFileLoader(HWAVEFILE handle, uint8_t size, uint8_t **buffer)
{
    static uint8_t buf_tmp[32];
    int bufferPos = 0;
    while(size --)
    {
        buf_tmp[bufferPos ++] = WaveFifo.read();
    }
    *buffer = buf_tmp;
    return 0;
}

static void WavBufferUpdate()
{
    while(WaveFifo.available() < WaveFifo.size() - 16)
    {
        uint8_t buffer[4];
        WavFile.read(buffer, sizeof(buffer));
        for(int i = 0; i < sizeof(buffer); i++)
        {
            WaveFifo.write(buffer[i]);
        }
    }
}

bool WavPlayer_LoadFile(String path)
{
    WavFile = SD.open(path, O_RDONLY);
    if(!WavFile)
    {
        return false;
    }
    
    /*支持的歌词文件类型*/
    const String LryicSupport_Grp[] = {".xtrc", ".xlrc", ".lrc"};
    path.replace(".wav", "");
    for(uint8_t i = 0; i < __Sizeof(LryicSupport_Grp); i++)
    {
        if(Lyric_Setup(path + LryicSupport_Grp[i]))
        {
            /*歌词文件匹配成功*/
            break;
        }
    }
    
    while(WaveFifo.available() < WaveFifo.size() - 16)
    {
        WavBufferUpdate();
    }

    Wav_StructInit(&Wav_Handle, WavFileLoader);
    Wav_Open(&Wav_Handle);

    Timer_SetInterrupt(XC_TIM_WAVPLAYER, 1000000 / Wav_Handle.Header.SampleFreq, WavTimer_Handler);
    Timer_SetInterruptFreqUpdate(XC_TIM_WAVPLAYER, Wav_Handle.Header.SampleFreq);
//    clock_out = Timer_GetClockOut(TIM_WAVPLAYER);
  
    return true;
}

void WavPlayer_SetEnable(bool en)
{
    Audio_SetEnable(en);
    TIM_Cmd(XC_TIM_WAVPLAYER, (FunctionalState)en);
    Wav_Playing = en;
    if(en)
    {
        /*获取到文件系统使用权*/
        if(xSemaphoreTake(SemHandle_FileSystem, 100) == pdTRUE)
        {
            xTaskNotifyGive(TaskHandle_WavPlayer);
        }
    }
    else
    {
        /*队列缓存清空*/
        WaveFifo.flush();
        /*结束标志位置位*/
        Wav_Handle.IsEnd = true;
    }
}

static void WavPlayer_AnimCallback(void * obj, int16_t volume)
{
    if(volume > 100)
        volume = 100;
    
    Wav_Volume = __Map(volume, 0, 100, 0.0f, WAV_VOLUME_MAX);
}

void WavPlayer_SetVolume(uint8_t volume)
{
    static lv_anim_t a;
    lv_obj_add_anim(
        NULL, &a,
        (lv_anim_exec_xcb_t)WavPlayer_AnimCallback,
        WavPlayer_GetVolume(), volume,
        1000
    );
}

uint8_t WavPlayer_GetVolume()
{
    return __Map(Wav_Volume, 0, WAV_VOLUME_MAX, 0, 100);
}

void WavPlayer_SetPlaying(bool en)
{
    Wav_Playing = en;
}

bool WavPlayer_GetPlaying()
{
    return Wav_Playing;
}

uint32_t WavPlayer_GetPlayTime()
{
    return Wav_Handle.DataPosition / Wav_Handle.Header.BytePerSecond;
}

const WAV_TypeDef* WavPlayer_GetWavHandle()
{
    return &Wav_Handle;
}

void Task_WavPlayer(void *pvParameters)
{
    /*检测SD卡是否插入*/
    pinMode(SD_CD_Pin, INPUT_PULLUP);
    if(digitalRead(SD_CD_Pin))
    {
        vTaskSuspend(TaskHandle_WavPlayer);
    }
    
//    /*检测SD卡是否初始化成功*/
//    if(!SD.begin(SD_CS_Pin, SD_SCK_MHZ(50))) 
//    {
//        vTaskSuspend(TaskHandle_WavPlayer);
//    }
    
    for(;;)
    {
        /*等待播放命令任务通知*/
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        /*缓冲区更新*/
        while(!Wav_Handle.IsEnd)
        {
            if(Wav_Playing)
            {
                uint32_t tick = Wav_Handle.DataPosition * 1000.0f / Wav_Handle.Header.BytePerSecond ;
                Lyric_Loop(tick);
                WavBufferUpdate();
                __IntervalExecute(FFT_Process(), 30);
            }
        }

        /*播放器失能*/
        WavPlayer_SetEnable(false);
        /*文件关闭*/
        WavFile.close();
        /*歌词解析器退出*/
        Lyric_Exit();
        
        /*归还文件系统使用权*/
        xSemaphoreGive(SemHandle_FileSystem);
    }
}
