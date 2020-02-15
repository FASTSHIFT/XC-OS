#include "Basic/FileGroup.h"
#include "DisplayPrivate.h"
#include "GamePrivate.h"

static lv_obj_t * appWindow;
static lv_obj_t * contGameDisp;
static lv_task_t * taskGame;
static lv_obj_t * btnGrp[MAX_BUTTON];
typedef struct{
    const char* sym;
    lv_align_t align;
    lv_coord_t x_mod;
    lv_coord_t y_mod;
}BtnCfg_TypeDef;
#define BTN_SIZE 45
static const BtnCfg_TypeDef btnCfg[MAX_BUTTON] = 
{
    {"A",             LV_ALIGN_IN_BOTTOM_RIGHT, -80, -60},
    {"B",             LV_ALIGN_IN_BOTTOM_RIGHT, -20, -120},
    {LV_SYMBOL_UP,    LV_ALIGN_IN_TOP_MID,        0,  5},
    {LV_SYMBOL_DOWN,  LV_ALIGN_IN_BOTTOM_MID,     0,  -5},
    {LV_SYMBOL_LEFT,  LV_ALIGN_IN_LEFT_MID,       5,  0},
    {LV_SYMBOL_RIGHT, LV_ALIGN_IN_RIGHT_MID,      -5,  0},
};

static void Task_GameScreenUpdate(lv_task_t * task)
{
    Game_DispTaskUpdate();
}

static void GameButton_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    for(int i = 0; i < MAX_BUTTON; i++)
    {
        if(obj == btnGrp[i])
        {
            if(event == LV_EVENT_PRESSED || event == LV_EVENT_PRESSING)
            {
                Game_SetButtonState(i, true);
            }
        }
    }
} 

static void Creat_GameButton()
{
    lv_obj_t * contBtn = lv_cont_create(appWindow, NULL);
    lv_cont_set_style(contBtn, LV_CONT_STYLE_MAIN, &lv_style_transp);
    lv_obj_set_size(contBtn, BTN_SIZE * 3 + 10, BTN_SIZE * 3 + 10);
    lv_obj_align(contBtn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 20, -BTN_SIZE - 10);
    
    for(int i = 0; i < MAX_BUTTON; i++)
    {
        btnGrp[i] = lv_btn_create((i >= UP_BUTTON) ? contBtn : appWindow, NULL);

        lv_obj_set_size(btnGrp[i], BTN_SIZE, BTN_SIZE);
        lv_obj_align(btnGrp[i], NULL, btnCfg[i].align, btnCfg[i].x_mod, btnCfg[i].y_mod);
        lv_obj_set_event_cb(btnGrp[i], GameButton_EventHandler);
        
        lv_obj_t * label = lv_label_create(btnGrp[i], NULL);
        lv_label_set_text(label, btnCfg[i].sym);
        
//        lv_btn_set_ink_in_time(btnGrp[i], 200);
//        lv_btn_set_ink_out_time(btnGrp[i], 200);
        lv_obj_set_protect(btnGrp[i], LV_PROTECT_PRESS_LOST);
    }
}

static void Creat_ContGameDisp()
{
    contGameDisp = lv_cont_create(appWindow, NULL);
    lv_cont_set_style(contGameDisp, LV_CONT_STYLE_MAIN, &lv_style_pretty_color);
    lv_obj_set_size(contGameDisp, GAME_SCREEN_WIDTH * 2 + 20, GAME_SCREEN_HEIGHT * 2 + 20);
    lv_obj_align(contGameDisp, NULL, LV_ALIGN_IN_TOP_MID, 0, 25);
}

void Game_DispGetContPos(int16_t* x, int16_t* y)
{
    *x = lv_obj_get_x(contGameDisp) + 10;
    *y = lv_obj_get_y(contGameDisp) + 10 + BarStatus_GetHeight();
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    lv_obj_move_foreground(appWindow);
    
    lv_obj_set_color(appWindow, LV_COLOR_BLACK);
    
    Creat_ContGameDisp();
    Creat_GameButton();
    
    taskGame = lv_task_create(Task_GameScreenUpdate, 20, LV_TASK_PRIO_HIGH, 0);
    
    Game_Init();
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    GamePage.Running();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    lv_task_del(taskGame);
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
void PageRegister_Game(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
