/**
  * XTRC ¸è´Ê½âÎöÆ÷
  * _VIFEXTech
  * Finish in 2019.11.17
  */

#ifndef __XTRC_ANALYZER_H
#define __XTRC_ANALYZER_H

#include "Arduino.h"

class XTRC_Analyzer{
    typedef String(*XTRC_GetNext_Callback_t)(void);
    typedef void(*XTRC_Update_Callback_t)(const char* str);
    
    /*[MM:SS.MS]<Duration>TEXT <Duration>TEXT <Duration>TEXT\r\n*/
    typedef struct{
        /*[MM:SS.MS]*/
        int Min, Sec, Ms;
        uint32_t TimeMs;
        /*<Duration>*/
        uint16_t Duration;
        /*TEXT*/
        String StrLrc;
    }LrcInfo_t;
    typedef enum{
        DoNothing,
        Error,
        WaitNowLine,
        WaitNextLrc,
        LoadBegin,
        LoadNextLine,
        LoadNextLrc
    }XTRC_Status_t;
    
public:
    LrcInfo_t LrcInfo;
    XTRC_Analyzer(
        XTRC_GetNext_Callback_t getNextCallback,
        XTRC_Update_Callback_t update_Callback
    );
    typedef enum{
        OutputMode_Single,
        OutputMode_Full
    }XTRC_OutputMode_t;
    typedef enum{
        DecodeMode_XTRC,
        DecodeMode_XLRC
    }XTRC_DecodeMode_t;
    void Start(
        XTRC_OutputMode_t outputmode = OutputMode_Single, 
        XTRC_DecodeMode_t decodemode = DecodeMode_XTRC
    );
    void Running(uint32_t tick = millis());
private:
    XTRC_GetNext_Callback_t GetNext_Callback;
    XTRC_Update_Callback_t Update_Callback;

    XTRC_Status_t NowStatus;
    XTRC_OutputMode_t OutputMode;
    XTRC_DecodeMode_t DecodeMode;
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
    void LrcDelay(uint32_t ms);
};

#endif
