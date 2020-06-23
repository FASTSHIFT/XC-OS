#include "Basic/TasksManage.h"
#include "HomePrivate.h"

static lv_obj_t * appWindow;

/*���˵���ͼ*/
static lv_obj_t * tabviewHome;

/*��tab*/
static uint8_t tabHomeIndex = 1;
/*һ��Tabview������Tab�����趨��ǩ��*/
static lv_obj_t * AppTab_Grp[3];

lv_obj_t * Home_GetAppWindow()
{
    return appWindow;
}

/**
  * @brief  ����Tabviewҳ��
  * @param  tabview: tabview�����ַ
  * @retval None
  */
static void Creat_Page(lv_obj_t** tabview)
{
    /*����Tabview*/
    *tabview = lv_tabview_create(appWindow, NULL);
    
    /*���ô�СΪӦ�ó�������С*/
    lv_obj_set_size(*tabview, APP_WIN_WIDTH, APP_WIN_HEIGHT);
    
    /*�붥������*/
    lv_obj_align(*tabview, appWindow, LV_ALIGN_IN_TOP_MID, 0, 0);
    
    /*�趨��ť����Ϊ���·�*/
    lv_tabview_set_btns_pos(*tabview, LV_TABVIEW_BTNS_POS_BOTTOM);
//    /*���ذ�ť*/
//    lv_tabview_set_btns_hidden(*tabview, true);
}

static void Home_Init()
{
    /*��ʼ����Tabviewҳ��*/
    Creat_Page(&tabviewHome);
    
    /*����Tab*/
    __LoopExecute(AppTab_Grp[i] = lv_tabview_add_tab(tabviewHome, ""), __Sizeof(AppTab_Grp));
    
    Home_AppInit(AppTab_Grp[tabHomeIndex]);
    
    /*Tab��������ҳ��*/
    lv_tabview_set_tab_act(tabviewHome, tabHomeIndex, LV_ANIM_OFF);
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    /*��ʾappWindow*/
    lv_obj_move_foreground(appWindow);
    lv_obj_set_hidden(appWindow, false);
    
    /*����Tab�飬ֻ��ʼ��һ��*/
    __ExecuteOnce(Home_Init());

    /*Ϊ��һ��APP�˳����Ŷ��������ǵ�һ�β�����*/
    static bool first = true;
    if(!first)
    {
        Home_AppAnim();
        Page_Delay(AnimCloseTime_Default);
    }
    first = false;//���б��
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    /*ΪAPP����������ʱ*/
    Page_Delay(AnimCloseTime_Default);
    
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
    /*��ȡ��������ť*/
    lv_obj_t * btn = (lv_obj_t*)param;
    /*�����¼�*/
    if(event == LV_EVENT_CLICKED)
    {
        /*�л�����Tab*/
        if(btn == btnHome)
        {
            lv_tabview_set_tab_act(tabviewHome, tabHomeIndex, LV_ANIM_ON);
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_Home(uint8_t pageID)
{
    appWindow = AppWindow_GetCont(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
