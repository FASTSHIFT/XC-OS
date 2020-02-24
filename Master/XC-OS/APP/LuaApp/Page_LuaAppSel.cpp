#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "GUI/Page/APP_Type.h"
#include "SdFat.h"
#include "Basic/TasksManage.h"
#include "BSP/BSP.h"

#if( XC_USE_LUA == 1 )
#  include "LuaInterface/LuaScript.h"
#endif

static lv_obj_t * appWindow;

#define APP_MAX 9
#define APP_FOLDER "/APP"
#define APP_ICON   "icon.bin"
#define APP_MAIN   "main.lua"

static APP_TypeDef APP_Grp[APP_MAX];

typedef struct{
    String path;
    String text;
}AppPathText_TypeDef;

static AppPathText_TypeDef AppPathText_Grp[APP_MAX];

static void APP_Call(const char *path)
{
#if ( XC_USE_LUA == 1 )
    LuaScriptExecuteFile(path);
#endif
//    page.PagePush(PAGE_LuaAppWin);
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
            char fileName[LV_FS_MAX_FN_LENGTH];
            file.getName(fileName, sizeof(fileName));
            
            String imgPath;
            imgPath.sprintf("S:%s/%s/"APP_ICON, path, fileName);
            
            AppPathText_Grp[count].path = imgPath;
            AppPathText_Grp[count].text = String(fileName);
            
            APP_Grp[count].img_dsc = AppPathText_Grp[count].path.c_str();
            APP_Grp[count].lable_text = AppPathText_Grp[count].text.c_str();
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
                luaPath.replace(APP_ICON, APP_MAIN);
                typedef void(*appcall_func_t)(const char*);
                ((appcall_func_t)APP_Grp[i].param)(luaPath.c_str() + 2);
            }
        }
    }
}

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

    int count = LoadFolderInfo(APP_FOLDER);
    
    /*��������Page�߳��ﴴ��APP����ͼƬ��ʾ��lvgl�߳��
     *����Ҫ���ٽ�������֤�������*/
    taskENTER_CRITICAL();
    __LoopExecute(Creat_APP(APP_Grp[i], appWindow, i, ImgbtnEvent_Handler), count);
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
void PageRegister_LuaAppSel(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    lv_style_t * style = (lv_style_t *)lv_cont_get_style(appWindow, LV_CONT_STYLE_MAIN);
    *style = lv_style_pretty;
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
