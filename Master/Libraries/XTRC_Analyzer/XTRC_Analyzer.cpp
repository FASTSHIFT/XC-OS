#include "XTRC_Analyzer.h"

//#define DEBUG_SERIAL Serial

#ifdef DEBUG_SERIAL
#define DEBUG(format, ...) DEBUG_SERIAL.printf(format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif

#define NowTime() (NowTick - StartTime)

XTRC_Analyzer::XTRC_Analyzer(
    XTRC_GetNext_Callback_t getNextCallback,
    XTRC_Update_Callback_t update_Callback)
{
    GetNext_Callback = getNextCallback;
    Update_Callback = update_Callback;
}

String XTRC_Analyzer::GetNextLineStr()
{
    return (GetNext_Callback ? GetNext_Callback() : "");
}

void XTRC_Analyzer::Update(String lrc)
{
    if(Update_Callback)
    {
        Update_Callback(lrc.c_str());
    }
}

bool XTRC_Analyzer::Begin()
{
    StrCurrent = GetNextLineStr();
    if(StrCurrent.startsWith("[offset"))
    {
        NowStatus = LoadNextLine;
        StartTime = NowTick;
        DEBUG("-->>begin(time%d)\r\n", NowTick);
        return true;
    }
    return false;
}

void XTRC_Analyzer::WaitNowTime(uint32_t ms)
{
    NowLineTime = ms;
    NowStatus = WaitNowLine;
}

void XTRC_Analyzer::LrcDelay(uint32_t ms)
{
    NextLrcTime = NowTime() + ms;
    NowStatus = (OutputMode == Mode_Single) ? WaitNextLrc : LoadNextLrc;
}

bool XTRC_Analyzer::AnalyzeCurrentLine()
{
    DEBUG("\r\n-------- Current:%s --------", StrCurrent.c_str());

    String StrTime = StrCurrent;
    int indexTime = StrTime.indexOf(']');
    
    if(indexTime == -1)
    {
        NowStatus = Error;
        return false;
    }
    
    StrTime = StrTime.substring(1, indexTime);
    StrCurrent = StrCurrent.substring(indexTime + 1);

    DEBUG(
        "\r\nTimeStr(%d):%s\r\nLrcStr:%s\r\n",
        indexTime, StrTime.c_str(), StrCurrent.c_str()
    );
    sscanf(StrTime.c_str(), "%d:%d.%d", &LrcInfo.Min, &LrcInfo.Sec, &LrcInfo.Ms);
    LrcInfo.TimeMs = LrcInfo.Min * 60 * 1000 + LrcInfo.Sec * 1000 + LrcInfo.Ms;
    //DEBUG("scTime %d:%d.%d\r\n", min, ss, ms);
    WaitNowTime(LrcInfo.TimeMs);

    return true;
}

bool XTRC_Analyzer::AnalyzeLrc()
{
    /*是否为最后一个单词*/
    bool isLastLrc = false;
    /*分离歌词*/
    int indexLrc = StrCurrent.indexOf('<', 1);
    if(indexLrc == -1)
    {
        int len = StrCurrent.length();
        if(len > 1)
        {
            isLastLrc = true;
            indexLrc = len;
        }
        else
        {
            DEBUG("-->>lrc end");
            return false;
        }
    }
    //DEBUG("indexLrc:%d\r\n",indexLrc);

    /*<时间>单词*/
    String StrLrc = StrCurrent.substring(1, indexLrc);
    //DEBUG("-|StrLrc:%s toInt() |-", StrLrc.c_str());
    /*时间*/
    int time_ms = StrLrc.toInt();
    /*歌词*/
    int from = StrLrc.indexOf('>');
    StrLrc = StrLrc.substring(from + 1);
    DEBUG("time:%dms,lrc:%s\r\n", time_ms, StrLrc.c_str());
    
    if(isLastLrc)
        StrLrc += "\r\n";
    Update(StrLrc);
    
    StrCurrent = StrCurrent.substring(indexLrc);
    LrcDelay(time_ms);
    return true;
}

void XTRC_Analyzer::Start(XTRC_OutputMode_t mode)
{
    OutputMode = mode;
    NowStatus = LoadBegin;
}

void XTRC_Analyzer::Running(uint32_t tick)
{
    NowTick = tick;
    switch(NowStatus)
    {
    case DoNothing:
        break;
    case Error:
        DEBUG("ERROR!");
        NowStatus = DoNothing;
        break;
    case WaitNowLine:
        /*行等待*/
        if(NowTime() > NowLineTime)
            NowStatus = LoadNextLrc;
        break;
    case WaitNextLrc:
        /*单词等待*/
        if(NowTime() > NextLrcTime)
            NowStatus = LoadNextLrc;
        break;
    case LoadBegin:
        Begin();
        break;
    case LoadNextLine:
        StrCurrent = GetNextLineStr();
        /*判断是否有新数据*/
        if(StrCurrent == "")
        {
            NowStatus = DoNothing;
            DEBUG("-->>END");
            break;
        }
        /*跳过中文歌词*/
        else if(StrCurrent.startsWith("[x-trans]"))
        {
            //DEBUG("-->>jump");
            break;
        }
        /*提取当行信息*/
        AnalyzeCurrentLine();
        break;
    case LoadNextLrc:
        /*单词分析*/
        if(!AnalyzeLrc())
        {
            NowStatus = LoadNextLine;
        }
        break;
    }
}
