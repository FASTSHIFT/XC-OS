#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "SettingsPrivate.h"

static lv_obj_t * appWindow;

static void ExitPageHook()
{
    page.PagePop();
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    lv_obj_move_foreground(appWindow);
    __ExecuteOnce(Creat_SettingsMain(appWindow));
    lv_settings_set_root_page_exit_callback(ExitPageHook);
    lv_obj_set_hidden(appWindow, false);
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_obj_set_hidden(appWindow, true);
}

/**
  * @brief  页面事件
  * @param  event:事件编号
  * @param  param:事件参数
  * @retval 无
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        if(btn == btnBack)
        {
            lv_settings_turn_back();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Settings(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
