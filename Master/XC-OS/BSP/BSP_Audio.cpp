#include "Basic/FileGroup.h"
#include "BSP.h"

#define USE_AUDIO_TONE 1

#define DAC_CH1_Pin PA4
#define DAC_CH2_Pin PA5

#define IS_DAC_Pin(Pin) (Pin==DAC_CH1_Pin||Pin==DAC_CH2_Pin)

static volatile uint32_t* Audio_RegisterBase = 0;

static void Audio_GetChannel1_RegisterBase(uint32_t DAC_Align)
{
#   define DHR12R1_OFFSET ((uint32_t)0x00000008)
    __IO uint32_t tmp = 0;

    /* Check the parameters */
    assert_param(IS_DAC_ALIGN(DAC_Align));
    assert_param(IS_DAC_DATA(Data));

    tmp = (uint32_t)DAC_BASE;
    tmp += DHR12R1_OFFSET + DAC_Align;
    Audio_RegisterBase = (volatile uint32_t*)tmp;
}

static void Audio_Init()
{
    pinMode(AudioMute_Pin, OUTPUT);

    if(!IS_DAC_Pin(Speaker_Pin))
        return;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    DAC_InitTypeDef dacConfig;
    DAC_StructInit(&dacConfig);
    dacConfig.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    dacConfig.DAC_Trigger = DAC_Trigger_None;
    dacConfig.DAC_WaveGeneration = DAC_WaveGeneration_None;
    if(Speaker_Pin == DAC_CH1_Pin)
    {
        DAC_Init(DAC_Channel_1, &dacConfig);
        DAC_Cmd(DAC_Channel_1, ENABLE);
        Audio_GetChannel1_RegisterBase(DAC_Align_12b_R);
    }
    else if(Speaker_Pin == DAC_CH2_Pin)
    {
        DAC_Init(DAC_Channel_2, &dacConfig);
        DAC_Cmd(DAC_Channel_2, ENABLE);
    }
    pinMode(Speaker_Pin, OUTPUT_AF);
}

void Audio_SetEnable(bool en)
{
    __ExecuteOnce(Audio_Init());

    digitalWrite(AudioMute_Pin, en);
}

void Audio_WriteData(uint16_t data)
{
    *Audio_RegisterBase = data;
}

#if (USE_AUDIO_TONE == 1)

#include "arm_math.h"

#define AUDUIO_TONE_SAMPLE_FREQ   50000
#define AUDUIO_TONE_HZ_TO_US(hz) (1000000.0f/(hz))
#define AUDUIO_TONE_TIM           TIM1

static const uint32_t Tone_SampleTime = AUDUIO_TONE_HZ_TO_US(AUDUIO_TONE_SAMPLE_FREQ);
static uint32_t Tone_SampleTick;
static uint32_t Tone_StopTime;
static bool Tone_AutoStop = false;
static float Tone_SignalPeriod;
static uint16_t Tone_Volume;

void Audio_ToneSetVolume(uint16_t volume)
{
    Tone_Volume = volume;
}

uint16_t Audio_ToneGetVolume()
{
    return Tone_Volume;
}

void Audio_NoTone()
{
    TIM_Cmd(AUDUIO_TONE_TIM, DISABLE);
}

static void Audio_ToneTimerCallback()
{
    if(Tone_AutoStop && millis() >= Tone_StopTime)
    {
        Audio_NoTone();
        return;
    }

    float fract = (Tone_SampleTick * Tone_SampleTime) / Tone_SignalPeriod;
    fract -= (int)fract;
    uint16_t output = arm_sin_q15(fract * 32767) + 32768;
    Audio_WriteData(output * ((float)Tone_Volume / 0xFFFF));
    Tone_SampleTick++;
}

static void Audio_ToneInit()
{
    Timer_SetInterrupt(AUDUIO_TONE_TIM, 0xFF, Audio_ToneTimerCallback);
    Timer_SetInterruptFreqUpdate(AUDUIO_TONE_TIM, AUDUIO_TONE_SAMPLE_FREQ);
}

void Audio_Tone(float freq)
{
    __ExecuteOnce(Audio_ToneInit());
    
    if(freq <= 1 || freq > 20000)
    {
        Audio_NoTone();
        return;
    }
    TIM_Cmd(AUDUIO_TONE_TIM, DISABLE);
    Tone_AutoStop = false;
    Tone_SampleTick = 0;
    Tone_SignalPeriod = AUDUIO_TONE_HZ_TO_US(freq);
    TIM_Cmd(AUDUIO_TONE_TIM, ENABLE);
}

void Audio_Tone(float freq, uint32_t time)
{
    if(freq <= 1 || freq > 20000 || time == 0)
    {
        Audio_NoTone();
        return;
    }
    Audio_Tone(freq);
    Tone_AutoStop = true;
    Tone_StopTime = millis() + time;
}

#endif
