#ifndef __MOUDLE_H
#define __MOUDLE_H

#include "stdint.h"
#include "Arduino.h"

/*Motor*/
void Motor_Vibrate(float strength, uint32_t time);

/*Power*/
extern float BattCurret, BattVoltage, BattVoltageOc;
void Power_Shutdown();

/*Audio*/
void Audio_SetEnable(bool en);
void WavPlayer_SetEnable(bool en);
void WavPlayer_SetVolume(uint8_t volume);
uint8_t WavPlayer_GetVolume();
void WavPlayer_SetPlaying(bool en);
bool WavPlayer_GetPlaying();
bool WavPlayer_LoadFile(String path);

/*Text*/
char *   TextBuf_GetBuff(uint32_t start = 0);
uint32_t TextBuf_GetSize();
void     TextBuf_Clear(uint8_t n = 0);

/*Brightness*/
uint16_t BrightnessGet();
void BrightnessSet(uint16_t val);

/*Xtrc*/
bool XTRC_Setup(String path);
void XTRC_Loop(uint32_t tick);
void XTRC_Exit();

#endif
