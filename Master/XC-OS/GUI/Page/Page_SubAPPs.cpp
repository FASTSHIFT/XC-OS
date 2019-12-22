#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "APP_Type.h"
#include "SdFat.h"
#include "TasksManage.h"
#include "Module.h"

#if( XC_USE_LUA == 1 )
#include "LuaScript.h"
#endif

static lv_obj_t * appWindow;

#define APP_MAX 9
#define APP_NAME_MAX 20
#define APP_PATH_MAX (APP_NAME_MAX+30)

typedef struct{
    char path[APP_PATH_MAX];
    char text[APP_NAME_MAX];
}AppPathText_TypeDef;

AppPathText_TypeDef AppPathText_Grp[APP_MAX];
APP_TypeDef APP_Grp[APP_MAX];

static void APP_Call(const char *path)
{
    LuaScriptExecuteFile(path);
}

static int LoadFolderInfo(const char *path)
{
    SdFile root, file;
    if (!root.open(path))
    {
        return 0;
    }

    int count = 0;
    while (file.openNext(&root, O_RDONLY))
    {
        if (!file.isHidden() && file.isDir())
        {
            char fileName[APP_NAME_MAX];
            file.getName(fileName, sizeof(fileName));
            
            char imgPath[APP_PATH_MAX];
            sprintf(imgPath, "S:%s/%s/%s.bin", path, fileName, fileName);
            
            strcpy(AppPathText_Grp[count].path, imgPath);
            strcpy(AppPathText_Grp[count].text, fileName);
            
            APP_Grp[count].img_dsc = AppPathText_Grp[count].path;
            APP_Grp[count].lable_text = AppPathText_Grp[count].text;
            APP_Grp[count].type = TYPE_FuncCall;
            APP_Grp[count].param = (int)APP_Call;
            
            count++;
        }
        file.close();
    }
    return count;
}

static void ImgbtnEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    /*�����¼�*/
    if(event == LV_EVENT_CLICKED)
    {
        for(int i = 0; i < __Sizeof(APP_Grp); i++)
        {
            if(obj == APP_Grp[i].imgbtn && APP_Grp[i].type == TYPE_FuncCall)
            {
                String luaPath = String((const char*)APP_Grp[i].img_dsc);
                luaPath.replace("bin", "lua");
                typedef void(*void_func_t)(const char*);
                ((void_func_t)APP_Grp[i].param)(luaPath.c_str() + 2);
            }
        }
    }
}

static lv_obj_t * contApp;

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    /*��ȡ���ļ�ϵͳʹ��Ȩ*/
    xSemaphoreTake(SemHandle_FileSystem, 1000);

    lv_obj_move_foreground(appWindow);
    contApp = lv_cont_create(appWindow, NULL);
    lv_cont_set_fit(contApp, LV_FIT_FLOOD);

    int count = LoadFolderInfo("/APP");
    
    /*��������Page�߳��ﴴ��APP����ͼƬ��ʾ��lvgl�߳��
     *����Ҫ���ٽ�������֤�������*/
    taskENTER_CRITICAL();
    __LoopExecute(Creat_APP(APP_Grp[i], contApp, i, ImgbtnEvent_Handler), count);
    taskEXIT_CRITICAL();
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    lv_obj_clean(appWindow);
#if( XC_USE_LUA == 1 )
    luaScript.end();
#endif
    /*�黹�ļ�ϵͳʹ��Ȩ*/
    xSemaphoreGive(SemHandle_FileSystem);
}

/**
  * @brief  ҳ���¼�
  * @param  event:�¼����
  * @param  param:�¼�����
  * @retval ��
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        if(btn == btnBack)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_SubAPPs(uint8_t pageID)
{
    appWindow = AppWindow_PageGet(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
