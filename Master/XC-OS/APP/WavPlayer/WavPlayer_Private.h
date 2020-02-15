#ifndef __WAVPLAYER_PRIVATE_H
#define __WAVPLAYER_PRIVATE_H

#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "WavDecoder/wav_decoder.h"
#include "BSP/BSP.h"
#include "SdFat.h"

/*Lyric*/
bool Lyric_Setup(String path);
void Lyric_Loop(uint32_t tick);
void Lyric_Exit();

/*FFT*/
void FFT_AddData(int32_t data);
void FFT_Process();
const float *FFT_GetStrength();

/*WavPlayer*/
void WavPlayer_SetEnable(bool en);
void WavPlayer_SetVolume(uint8_t volume);
uint8_t WavPlayer_GetVolume();
void WavPlayer_SetPlaying(bool en);
bool WavPlayer_GetPlaying();
bool WavPlayer_LoadFile(String path);
uint32_t WavPlayer_GetPlayTime();
const WAV_TypeDef* WavPlayer_GetWavHandle();

void Wav_LabelLrcUpdate(const char* str);

/*MusicList*/
lv_obj_t * MusicList_GetList();
void MusicList_SetActivate(bool act);
bool MusicList_GetActivate();
String MusicList_GetNextMusic(bool next);
String MusicList_GetNowMusic();
void MusicList_PlayNext(bool next);
void MusicList_Init(const char* path);

#endif
