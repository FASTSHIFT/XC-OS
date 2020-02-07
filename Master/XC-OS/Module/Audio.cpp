#include "Basic/FileGroup.h"
#include "Module.h"

#define DAC_CH1_Pin PA4
#define DAC_CH2_Pin PA5

#define IS_DAC_Pin(Pin) (Pin==DAC_CH1_Pin||Pin==DAC_CH2_Pin)

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
