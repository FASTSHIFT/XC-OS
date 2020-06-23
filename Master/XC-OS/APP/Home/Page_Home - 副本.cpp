#include "Basic/FileGroup.h"
#include "DisplayPrivate.h"
#include "Basic/TasksManage.h"
#include "APP_Type.h"
#include "BSP/BSP.h"

static lv_obj_t * appWindow;

/*���˵���ͼ*/
static lv_obj_t * tabviewHome;

/*����������ҳ����ת����*/
static lv_obj_t * contAppSw;
/*����ʱ��*/
#define AnimCloseTime_Default 200
static void AppSwitch_AnimClose(bool close = true, lv_coord_t x = 0, lv_coord_t y = 0,uint16_t time = AnimCloseTime_Default);

/*���APP���Tab*/
typedef struct
{
    const char* text;
    lv_obj_t * tab;
} AppTab_TypeDef;

/*��tab*/
static uint8_t tabHomeIndex = 1;
/*һ��Tabview������Tab�����趨��ǩ��*/
static AppTab_TypeDef AppTab_Grp[] =
{
    {""},
    {""},
    {""}
};

/*ͼƬ�ļ�*/
LV_IMG_DECLARE(ImgSettings);
LV_IMG_DECLARE(ImgCode);
LV_IMG_DECLARE(ImgGame);
LV_IMG_DECLARE(ImgSubApps);
LV_IMG_DECLARE(ImgFiles);
LV_IMG_DECLARE(ImgEditor);
LV_IMG_DECLARE(ImgVideo);
LV_IMG_DECLARE(ImgMusic);
LV_IMG_DECLARE(ImgTerminal);

/*APP��*/
static APP_TypeDef APP_Grp[] =
{
    {&ImgSettings, "Settings", TYPE_PageJump, PAGE_Settings},
    {&ImgCode,     "IDE",      TYPE_PageJump, PAGE_LuaScript},
    {&ImgGame,     "Game",     TYPE_PageJump, PAGE_GameSelect},
    {&ImgSubApps,  "APPs",     TYPE_PageJump, PAGE_LuaAppSel},
    {&ImgFiles,    "Files",    TYPE_PageJump, PAGE_FileExplorer},
    {&ImgVideo,    "Video",    TYPE_PageJump, PAGE_BvPlayer},
    {&ImgEditor,   "Editor",   TYPE_PageJump, PAGE_TextEditor},
    {&ImgMusic,    "Music",    TYPE_PageJump, PAGE_WavPlayer},
    {&ImgTerminal, "Shell",    TYPE_PageJump, PAGE_Shell},
};

/**
  * @brief  APPͼƬ�¼�
  * @param  obj:����
  * @param  event:�¼�����
  * @retval None
  */
static void ImgbtnEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    /*�����¼�*/
    if(event == LV_EVENT_CLICKED)
    {
        /*ɨ��APP��*/
        for(int i = 0; i < __Sizeof(APP_Grp); i++)
        {
            if(obj == APP_Grp[i].imgbtn && APP_Grp[i].type == TYPE_PageJump)
            {
                /*ҳ����ת*/
                page.PagePush(APP_Grp[i].param);
                /*������ת����*/
                AppSwitch_AnimClose(
                    false,
                    lv_obj_get_x_center(APP_Grp[i].cont),
                    lv_obj_get_y_center(APP_Grp[i].cont),
                    AnimCloseTime_Default
                );
            }
        }
    }
    if(event == LV_EVENT_LONG_PRESSED)
    {
        Motor_Vibrate(1, 20);
    }
}

/**
  * @brief  ����APPͼ��
  * @param  app:APP����
  * @param  parent:����APPͼ��ĸ�����
  * @param  index:����0~9
  * @retval None
  */
void Creat_APP(APP_TypeDef &app, lv_obj_t * parent, uint8_t index, lv_event_cb_t imgbtn_event_handler)
{
    /*��ֹ����Խ��*/
    if(index > 8)
        return;
    
    /*����APPͼ������*/
    app.cont = lv_cont_create(parent, NULL);
    /*��ʽ����Ϊ͸��*/
    lv_cont_set_style(app.cont, LV_CONT_STYLE_MAIN, &lv_style_transp);

    /*����ͼ�갴ť�ؼ�*/
    app.imgbtn = lv_imgbtn_create(app.cont, NULL);
    lv_imgbtn_set_src(app.imgbtn, LV_BTN_STATE_REL, app.img_dsc);
    lv_imgbtn_set_src(app.imgbtn, LV_BTN_STATE_PR, app.img_dsc);
    lv_imgbtn_set_toggle(app.imgbtn, false);
    /*ͼƬ��������ʽ*/
    static lv_style_t style_pr = *lv_imgbtn_get_style(app.imgbtn, LV_IMGBTN_STYLE_PR);
    style_pr.image.color = LV_COLOR_GRAY;
    style_pr.image.intense = LV_OPA_70;
    lv_imgbtn_set_style(app.imgbtn, LV_IMGBTN_STYLE_PR, &style_pr);
    /*�����¼�*/
    lv_obj_set_event_cb(app.imgbtn, imgbtn_event_handler);

    /*��ͼ���´�����ǩ*/
    app.label = lv_label_create(app.cont, NULL);
    lv_label_set_text(app.label, app.lable_text);
    /*���ж���*/
    lv_obj_align(app.label, app.imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    /*������С����Ӧ*/
    lv_cont_set_fit(app.cont, LV_FIT_TIGHT);
    lv_cont_set_layout(app.cont, LV_LAYOUT_COL_M);

    /*APP��������*/
    #define MOD_OFFSET 20//��Եƫ����
    const lv_coord_t mod_grp[3] = {MOD_OFFSET, 0, -MOD_OFFSET};
    /*����ģʽ��*/
    const lv_align_t align_grp[] = {
        LV_ALIGN_IN_TOP_LEFT,
        LV_ALIGN_IN_TOP_MID,
        LV_ALIGN_IN_TOP_RIGHT,
        LV_ALIGN_IN_LEFT_MID,
        LV_ALIGN_CENTER,
        LV_ALIGN_IN_RIGHT_MID,
        LV_ALIGN_IN_BOTTOM_LEFT,
        LV_ALIGN_IN_BOTTOM_MID,
        LV_ALIGN_IN_BOTTOM_RIGHT
    };
    lv_obj_align(
        app.cont, 
        parent, 
        align_grp[index], 
        mod_grp[index % 3], 
        mod_grp[index / 3]
    );
}

/**
  * @brief  ���ɴ��APP���Tab
  * @param  tabview:������
  * @param  apptab:APP��������
  * @retval None
  */
static void Creat_Tab(lv_obj_t* tabview, AppTab_TypeDef &apptab)
{
    /*���Tab*/
    apptab.tab = lv_tabview_add_tab(tabview, apptab.text);
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
    //lv_tabview_set_btns_hidden(*tabview, true);
}

/**
  * @brief  APP�л���������
  * @param  close: �Ƿ�Ϊ�رն���
  * @param  x:��ʼ����X(����������Ч)
  * @param  y:��ʼ����Y(����������Ч)
  * @param  time:����ʱ��(ms)
  * @retval None
  */
static void AppSwitch_AnimClose(bool close , lv_coord_t x , lv_coord_t y ,uint16_t time)
{
    /*������������*/
    if(!contAppSw)
    {
        contAppSw = lv_cont_create(appWindow, NULL);
    }
    
    /*���ڶ���*/
    lv_obj_set_top(contAppSw, true);
    
    /*�ر�����*/
    lv_obj_set_hidden(contAppSw, false);
    
    /*������*/
    enum{A_X, A_Y, A_W, A_H, A_OPA, A_MAX};
    static lv_anim_t anim_grp[A_MAX];
    
    /*�������ʼ����ִֻ��һ��*/
    __ExecuteOnce((  
        /*�������͸����*/
        lv_obj_set_opa_scale_enable(contAppSw, true),
        
        /*����*/
        anim_grp[0].var = contAppSw,
        /*·��*/
        anim_grp[0].path_cb  = lv_anim_path_ease_in_out,
        /*ʱ��*/
        anim_grp[0].time = time,
        /*������ͬ����*/
        anim_grp[1] = anim_grp[0],
        anim_grp[2] = anim_grp[0],
        anim_grp[3] = anim_grp[0],
        anim_grp[4] = anim_grp[0],
        
        /*�����ص��趨*/
        anim_grp[A_X].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x,
        anim_grp[A_Y].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y,
        anim_grp[A_W].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_width,
        anim_grp[A_H].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_height,
        anim_grp[A_OPA].exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_opa_scale
    ));

    /*�Ƿ�Ϊ�رն���*/
    if(!close)
    {
        /*����յ��趨*/
        anim_grp[A_OPA].start = LV_OPA_TRANSP;
        anim_grp[A_OPA].end = LV_OPA_COVER;
        
        anim_grp[A_W].start = anim_grp[A_H].start = 0;
        anim_grp[A_W].end = lv_obj_get_width(appWindow);
        anim_grp[A_H].end = lv_obj_get_height(appWindow);
        
        anim_grp[A_X].start = x;
        anim_grp[A_Y].start = y;
        
        anim_grp[A_X].end = 0;
        anim_grp[A_Y].end = 0;
    }
    else
    {
        /*��ת����յ��趨������*/
#define ANIM_SWAP(anim) {int temp;temp=anim.start,anim.start=anim.end,anim.end=temp;}
        __LoopExecute(ANIM_SWAP(anim_grp[i]), __Sizeof(anim_grp));
    }
    
    /*Ӧ�ö�����*/
    __LoopExecute(lv_anim_create(&(anim_grp[i])), __Sizeof(anim_grp));
}

static void HomeInit()
{
    /*��ʼ����Tabviewҳ��*/
    Creat_Page(&tabviewHome);
    
    /*����Tab*/
    __LoopExecute(Creat_Tab(tabviewHome, AppTab_Grp[i]), __Sizeof(AppTab_Grp));
    
    /*����Tab����APP��*/
    __LoopExecute(Creat_APP(APP_Grp[i], AppTab_Grp[tabHomeIndex].tab, i, ImgbtnEvent_Handler), __Sizeof(APP_Grp));
    
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
    __ExecuteOnce(HomeInit());

    /*Ϊ��һ��APP�˳����Ŷ��������ǵ�һ�β�����*/
    static bool first = true;
    if(!first)
    {
        AppSwitch_AnimClose();
        vTaskDelay(AnimCloseTime_Default);
        lv_obj_set_hidden(contAppSw, true);
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
    vTaskDelay(AnimCloseTime_Default);
    
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
