#ifndef __BSP_H
#define __BSP_H

#include "Arduino.h"

/*Motor*/
void Motor_SetEnable(bool en);
void Motor_Vibrate(float strength, uint32_t time);
void Motor_SetState(bool state);

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

/*MemoryPool*/
void* MemPool_Malloc(uint32_t size);
void MemPool_Free(void* pData);
uint32_t MemPool_GetResidueSize();
void MemPool_Clear(uint8_t n = 0);

/*Backlight*/
uint16_t Backlight_GetValue();
void Backlight_SetValue(int16_t val);
void Backlight_SetGradual(uint16_t target, uint16_t time = 500);

#endif
