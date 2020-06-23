#include "Basic/TasksManage.h"
#include "HomePrivate.h"

static lv_obj_t * appWindow;

/*主菜单视图*/
static lv_obj_t * tabviewHome;

/*主tab*/
static uint8_t tabHomeIndex = 1;
/*一个Tabview放三个Tab，不设定标签名*/
static lv_obj_t * AppTab_Grp[3];

lv_obj_t * Home_GetAppWindow()
{
    return appWindow;
}

/**
  * @brief  生成Tabview页面
  * @param  tabview: tabview对象地址
  * @retval None
  */
static void Creat_Page(lv_obj_t** tabview)
{
    /*创建Tabview*/
    *tabview = lv_tabview_create(appWindow, NULL);
    
    /*设置大小为应用程序区大小*/
    lv_obj_set_size(*tabview, APP_WIN_WIDTH, APP_WIN_HEIGHT);
    
    /*与顶部对齐*/
    lv_obj_align(*tabview, appWindow, LV_ALIGN_IN_TOP_MID, 0, 0);
    
    /*设定按钮方向为在下方*/
    lv_tabview_set_btns_pos(*tabview, LV_TABVIEW_BTNS_POS_BOTTOM);
//    /*隐藏按钮*/
//    lv_tabview_set_btns_hidden(*tabview, true);
}

static void Home_Init()
{
    /*初始化主Tabview页面*/
    Creat_Page(&tabviewHome);
    
    /*创建Tab*/
    __LoopExecute(AppTab_Grp[i] = lv_tabview_add_tab(tabviewHome, ""), __Sizeof(AppTab_Grp));
    
    Home_AppInit(AppTab_Grp[tabHomeIndex]);
    
    /*Tab滑动至主页面*/
    lv_tabview_set_tab_act(tabviewHome, tabHomeIndex, LV_ANIM_OFF);
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    /*显示appWindow*/
    lv_obj_move_foreground(appWindow);
    lv_obj_set_hidden(appWindow, false);
    
    /*创建Tab组，只初始化一次*/
    __ExecuteOnce(Home_Init());

    /*为上一个APP退出播放动画，但是第一次不运行*/
    static bool first = true;
    if(!first)
    {
        Home_AppAnim();
        Page_Delay(AnimCloseTime_Default);
    }
    first = false;//这行别改
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    /*为APP开启动画延时*/
    Page_Delay(AnimCloseTime_Default);
    
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
    /*获取导航栏按钮*/
    lv_obj_t * btn = (lv_obj_t*)param;
    /*单击事件*/
    if(event == LV_EVENT_CLICKED)
    {
        /*切换到主Tab*/
        if(btn == btnHome)
        {
            lv_tabview_set_tab_act(tabviewHome, tabHomeIndex, LV_ANIM_ON);
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_Home(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
