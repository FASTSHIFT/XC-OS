#ifndef __MOUDLE_H
#define __MOUDLE_H

#include "Arduino.h"

/*Motor*/
void Motor_Vibrate(float strength, uint32_t time);

/*Power*/
void Power_Shutdown();
void Power_GetInfo(float* battCurrent,float* battVoltage,float* battVoltageOc);

/*Audio*/
void Audio_SetEnable(bool en);

/*MusicWav*/
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
uint16_t Brightness_GetValue();
void Brightness_SetValue(int16_t val);
void Brightness_SetGradual(uint16_t target, uint16_t time = 500);

/*Lyric*/
bool Lyric_Setup(String path);
void Lyric_Loop(uint32_t tick);
void Lyric_Exit();

/*FFT*/
void FFT_AddData(int32_t data);
void FFT_Process();
const float *FFT_GetStrength();

#endif
