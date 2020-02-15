#include "GUI/DisplayPrivate.h"
#include "USB_Private.h"

static lv_obj_t * appWindow;

static void Setup()
{
    lv_obj_move_foreground(appWindow);
    Shell_Init();
}

static void Loop()
{
    shell_task();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    shell_println("\r\nshell exit...");
    shell_unregister_all();
    lv_obj_clean(appWindow);
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
            page.PagePop();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_USB(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
