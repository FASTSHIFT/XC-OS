/**
  * 歌词解析器(支持XTRC、XLRC、LRC歌词文件)
  * _VIFEXTech
  * Finish in 2019.11.17
  */

#ifndef __LYRIC_ANALYZER_H
#define __LYRIC_ANALYZER_H

#include "Arduino.h"

class Lyric_Analyzer{
    typedef String(*Lyric_GetNext_Callback_t)(void);
    typedef void(*Lyric_Update_Callback_t)(const char* str);
    
    /*[MM:SS.MS]<Duration>TEXT <Duration>TEXT <Duration>TEXT\r\n*/
    typedef struct{
        /*[MM:SS.MS]*/
        int Min, Sec, Ms;
        uint32_t TimeMs;
        /*<Duration>*/
        uint16_t Duration;
        /*TEXT*/
        String StrLyric;
    }LyricInfo_t;
    typedef enum{
        DoNothing,
        Error,
        WaitNowLine,
        WaitNextLrc,
        LoadBegin,
        LoadNextLine,
        LoadNextLrc
    }Lyric_Status_t;
    
public:
    LyricInfo_t LrcInfo;
    Lyric_Analyzer(
        Lyric_GetNext_Callback_t getNextCallback,
        Lyric_Update_Callback_t update_Callback
    );
    typedef enum{
        OutputMode_Single,
        OutputMode_Full
    }Lyric_OutputMode_t;
    typedef enum{
        DecodeMode_XTRC,
        DecodeMode_XLRC
    }Lyric_DecodeMode_t;
    void Start(
        Lyric_OutputMode_t outputmode = OutputMode_Single, 
        Lyric_DecodeMode_t decodemode = DecodeMode_XTRC
    );
    void Running(uint32_t tick = millis());
private:
    Lyric_GetNext_Callback_t GetNext_Callback;
    Lyric_Update_Callback_t Update_Callback;

    Lyric_Status_t NowStatus;
    Lyric_OutputMode_t OutputMode;
    Lyric_DecodeMode_t DecodeMode;
    uint32_t StartTime;
    uint32_t NowLineTime;
    uint32_t NextLrcTime;
    uint32_t NowTick;
    
    String StrCurrent;
    bool StrCurrent_BeginLoaded;

    String GetNextLineStr();
    bool AnalyzeCurrentLine();
    bool AnalyzeXtrcLrc();
    void Update(String lrc);

    bool Begin();
    void WaitNowTime(uint32_t ms);
    void LyricDelay(uint32_t ms);
};

#endif
