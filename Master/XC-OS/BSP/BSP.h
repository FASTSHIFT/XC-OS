#ifndef __BSP_H
#define __BSP_H

#include "Arduino.h"

/*Motor*/
void Motor_SetEnable(bool en);
void Motor_Vibrate(float strength, uint32_t time);

/*Power*/
void Power_Shutdown();
void Power_GetInfo(float* battCurrent,float* battVoltage,float* battVoltageOc);
float Power_GetBattUsage();

/*Audio*/
void Audio_SetEnable(bool en);
void Audio_WriteData(uint16_t data);
void Audio_ToneSetVolume(uint16_t volume);
uint16_t Audio_ToneGetVolume();
void Audio_NoTone();
void Audio_Tone(float freq);
void Audio_Tone(float freq, uint32_t time);

/*TextBuf*/
char *   TextBuf_GetBuff(uint32_t start = 0);
uint32_t TextBuf_GetSize();
void     TextBuf_Clear(uint8_t n = 0);

/*Brightness*/
uint16_t Brightness_GetValue();
void Brightness_SetValue(int16_t val);
void Brightness_SetGradual(uint16_t target, uint16_t time = 500);

#endif
