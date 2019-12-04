#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "doom-nano\doom_nano.h"

static lv_task_t * task_doom;
static lv_style_t appWinStyle,gameWinStyle;
extern uint8_t doomKey[K_MAX];
static lv_obj_t * btnm;
static const char * btnm_map[] = {
             LV_SYMBOL_UP, "\n",
    LV_SYMBOL_LEFT, " FIRE ", LV_SYMBOL_RIGHT, "\n",
            LV_SYMBOL_DOWN, ""
};

static void Task_DoomUpdate(lv_task_t * task)
{
    doom_updateScreen();
}

static void DoomKeys_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    uint8_t bin_id = lv_btnm_get_active_btn(obj);
    if(event == LV_EVENT_PRESSED)
    {
        doomKey[bin_id] = true;
    }
    else if(event == LV_EVENT_RELEASED)
    {
        doomKey[bin_id] = false;
    }
} 

static void Creat_DoomKeys()
{
    btnm = lv_btnm_create(appWindow, NULL);
    lv_btnm_set_map(btnm, btnm_map);
    lv_obj_align(btnm, barNavigation, LV_ALIGN_OUT_TOP_MID, 0, - 40);
    lv_obj_set_event_cb(btnm, DoomKeys_EventHandler);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    /*备份style*/
    appWinStyle = *lv_cont_get_style(appWindow, LV_CONT_STYLE_MAIN);
    
    gameWinStyle = lv_style_plain;
    gameWinStyle.body.grad_color = gameWinStyle.body.main_color = LV_COLOR_BLACK;
    gameWinStyle.body.opa = LV_OPA_COVER;
    lv_cont_set_style(appWindow, LV_CONT_STYLE_MAIN, &gameWinStyle);
    
    Creat_DoomKeys();
    task_doom = lv_task_create(Task_DoomUpdate, 10, LV_TASK_PRIO_HIGH, 0);
    
    doom_setup();
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    doom_loop();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_task_del(task_doom);
    lv_obj_del_safe(&btnm);
    lv_cont_set_style(appWindow, LV_CONT_STYLE_MAIN, &appWinStyle);
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
void PageRegister_Game(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
