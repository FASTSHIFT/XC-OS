#include "FileGroup.h"
#include "Module.h"
#include "TasksManage.h"
#include "FifoQueue.h"
#include "wav_decoder.h"
#include "SdFat.h"

/*WAV�������*/
#define FIFO_Size 6000
char waveBuff[FIFO_Size];
FifoQueue<char> WaveFifo(sizeof(waveBuff), waveBuff);

/*WAV�ļ���Ϣ*/
File WavFile;
WAV_TypeDef Wav_Handle;

/*WAV���ſ���*/
#define WAV_VOLUME_MAX 0.7f
static float Wav_Volume = WAV_VOLUME_MAX;
static bool Wav_Playing = true;

/*WAV����������*/
TaskHandle_t TaskHandle_WavPlayer;

static void WavSendDataToDAC()
{
    int WavAvailable = WaveFifo.available();
    if(WavAvailable < 16)
    {
        return;
    }
    else
    {
        uint16_t data = ((Wav_Handle.CurrentData.LeftVal + 32768) >> 6) * Wav_Volume;
        Wav_Next_16Bit2Channel(&Wav_Handle);
        DAC_SetChannel1Data(DAC_Align_12b_R, data);
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
    while(WaveFifo.available() < FIFO_Size - 16)
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
    
    /*֧�ֵĸ���ļ�����*/
    const String LryicSupport_Grp[] = {".xtrc", ".xlrc", ".lrc"};
    path.replace(".wav", "");
    for(uint8_t i = 0; i < __Sizeof(LryicSupport_Grp); i++)
    {
        if(Lyric_Setup(path + LryicSupport_Grp[i]))
        {
            /*����ļ�ƥ��ɹ�*/
            break;
        }
    }
    
    while(WaveFifo.available() < FIFO_Size - 16)
    {
        WavBufferUpdate();
    }

    Wav_StructInit(&Wav_Handle, WavFileLoader);
    Wav_Open(&Wav_Handle);

    Timer_SetInterrupt(XC_TIM_WAVPLAYER, 1000000 / Wav_Handle.Header.SampleFreq, WavSendDataToDAC);
    Timer_SetInterruptFreqUpdate(XC_TIM_WAVPLAYER, Wav_Handle.Header.SampleFreq);
//    clock_out = Timer_GetClockOut(TIM_WAVPLAYER);
  
    return true;
}

void WavPlayer_SetEnable(bool en)
{
    Audio_SetEnable(en);
    TIM_Cmd(XC_TIM_WAVPLAYER, (FunctionalState)en);
    if(en)
    {
        xTaskNotifyGive(TaskHandle_WavPlayer);
    }
    else
    {
        WaveFifo.flush();
        Wav_Handle.IsEnd = true;
    }
}

void WavPlayer_SetVolume(uint8_t volume)
{
    if(volume > 100)
        volume = 100;
    
    Wav_Volume = __Map(volume, 0, 100, 0.0f, WAV_VOLUME_MAX);
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

void Task_WavPlayer(void *pvParameters)
{
    /*���SD���Ƿ����*/
    pinMode(SD_CD_Pin, INPUT_PULLUP);
    if(digitalRead(SD_CD_Pin))
    {
        vTaskSuspend(TaskHandle_WavPlayer);
    }
    
    /*���SD���Ƿ��ʼ���ɹ�*/
    if(!SD.begin(SD_CS_Pin, SD_SCK_MHZ(50))) 
    {
        vTaskSuspend(TaskHandle_WavPlayer);
    }
    
    for(;;)
    {
        /*�ȴ�������������֪ͨ*/
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        /*����������*/
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

        /*������ʧ��*/
        WavPlayer_SetEnable(false);
        /*�ļ��ر�*/
        WavFile.close();
        /*��ʽ������˳�*/
        Lyric_Exit();
    }
}
