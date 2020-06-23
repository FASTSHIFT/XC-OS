#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "SettingsPrivate.h"

static lv_obj_t * appWindow;

static void ExitPageHook()
{
    page.PagePop();
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    lv_obj_move_foreground(appWindow);
    __ExecuteOnce(Creat_SettingsMain(appWindow));
    lv_settings_set_root_page_exit_callback(ExitPageHook);
    lv_obj_set_hidden(appWindow, false);
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    lv_obj_set_hidden(appWindow, true);
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
            lv_settings_turn_back();
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_Settings(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
