#include "Module.h"
#include "SdFat.h"
#include "XTRC_Analyzer.h"

static File XTRC_File;
static bool HaveFile = false;

/*���ļ���ȡһ��*/
static String XTRC_GetNextLine()
{
    return XTRC_File.readStringUntil('\n');
}

extern void Task_LabelLrcUpdate(const char* str);

/*��ʸ����¼�*/
static void XTRC_UpdateEvent(const char* str)
{
    Task_LabelLrcUpdate(str);
//    Serial.print(str);
}

static XTRC_Analyzer xtrc(XTRC_GetNextLine, XTRC_UpdateEvent);

bool XTRC_Setup(String path)
{
    HaveFile = false;
    int index = path.lastIndexOf('/');
    String filename = path.substring(index);
    path = path.substring(0, index) + "/lyrics" + filename;
    
    XTRC_File = SD.open(path);
    if(XTRC_File)
    {
        xtrc.Start();
        HaveFile = true;
    }
    return HaveFile;
}

void XTRC_Loop(uint32_t tick)
{
    if(HaveFile)
    {
        xtrc.Running(tick);
    }
}

void XTRC_Exit()
{
    XTRC_File.close();
}
