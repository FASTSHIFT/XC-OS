#include "Module.h"
#include "SdFat.h"
#include "Lyric_Analyzer.h"

static File LyricFile;
static bool HaveLyricFile = false;

/*从文件读取一行*/
static String Lyric_GetNextLine()
{
    return LyricFile.readStringUntil('\n');
}

extern void Wav_LabelLrcUpdate(const char* str);

/*歌词更新事件*/
static void Lyric_UpdateEvent(const char* str)
{
    Wav_LabelLrcUpdate(str);
//    Serial.print(str);
}

/*实例化歌词解析器对象*/
static Lyric_Analyzer lyric(Lyric_GetNextLine, Lyric_UpdateEvent);

bool Lyric_Setup(String path)
{
    HaveLyricFile = false;
    int index = path.lastIndexOf('/');
    if(index == -1)
        return false;
    
    String filename = path.substring(index);
    path = path.substring(0, index) + "/lyrics" + filename;
    
    LyricFile = SD.open(path);
    
    if(!LyricFile)
        return false;

    LyricFile.setTimeout(5);
    
    if(path.endsWith(".xtrc"))
    {
        lyric.Start();
    }
    else if(path.endsWith(".xlrc") || path.endsWith(".lrc"))
    {
        lyric.Start(lyric.OutputMode_Full, lyric.DecodeMode_XLRC);
    }
    else
    {
        LyricFile.close();
        return false;
    }

    HaveLyricFile = true;
    return true;
}

void Lyric_Loop(uint32_t tick)
{
    if(HaveLyricFile)
    {
        lyric.Running(tick);
    }
}

void Lyric_Exit()
{
    LyricFile.close();
}
