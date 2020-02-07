#include "WavPlayer_Private.h"
#include "Lyric_Analyzer/Lyric_Analyzer.h"

#define LyricFloder "/lyrics"

static File LyricFile;
static bool HaveLyricFile = false;

/*从文件读取一行*/
static String Lyric_GetNextLine()
{
    return LyricFile.readStringUntil('\n');
}

/*实例化歌词解析器对象*/
static Lyric_Analyzer lyric(Lyric_GetNextLine, Wav_LabelLrcUpdate);

bool Lyric_Setup(String path)
{
    HaveLyricFile = false;
    int index = path.lastIndexOf('/');
    if(index == -1)
        return false;
    
    String filename = path.substring(index);
    path = path.substring(0, index) + LyricFloder + filename;
    
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
