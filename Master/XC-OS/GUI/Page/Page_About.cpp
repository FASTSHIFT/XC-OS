#include "Basic/FileGroup.h"
#include "DisplayPrivate.h"
#include "APP_Type.h"

static lv_obj_t * appWindow;
static lv_obj_t * winAbout;
static lv_obj_t * listItems;
static lv_obj_t * listItems_btn;

static void WinAbout_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_DELETE)
    {
        page.PagePop();
    }
}

static void Creat_WinAbout()
{
    winAbout = lv_win_create(appWindow, NULL);
    lv_obj_align(winAbout, appWindow, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(winAbout, WinAbout_EventHandler);
    
    lv_win_set_title(winAbout, " About");
    
    lv_obj_t * close_btn = lv_win_add_btn(winAbout, LV_SYMBOL_CLOSE);
    lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);
    
    lv_win_set_btn_size(winAbout, 40);
}

/*图片文件*/
LV_IMG_DECLARE(ImgOS);
LV_IMG_DECLARE(ImgCPU);
LV_IMG_DECLARE(ImgRAM);
LV_IMG_DECLARE(ImgROM);
LV_IMG_DECLARE(ImgBattery);

static const ListBtn_TypeDef ListBtn_Grp[]={
    {&ImgOS,  "OS: "XC_OS_NAME" "XC_VERSION_HW" "XC_VERSION_SW},
    {&ImgCPU, "CPU: "XC_CPU_NAME" @%dMHz",     TYPE_Number, XC_CPU_RATE_MHZ},
    {&ImgRAM, "RAM: %dKB",                     TYPE_Number, XC_RAM_SIZE/1024},
    {&ImgROM, "ROM: %dKB",                     TYPE_Number, XC_ROM_SIZE/1024},
    {&ImgBattery, "BATT: "XC_BATTERY_NAME" @%dmAh", TYPE_Number, XC_BATTERY_CAPACITY_MAH},
};

static void Creat_ListBtn()
{
    String str_tmp;
    /*Add buttons to the list*/
    for(int i = 0; i < __Sizeof(ListBtn_Grp); i++)
    {
        if(ListBtn_Grp[i].type == TYPE_Number)
        {
            str_tmp.sprintf(ListBtn_Grp[i].text, ListBtn_Grp[i].param);
        }
        else
        {
            str_tmp = String(ListBtn_Grp[i].text);
        }
        listItems_btn = lv_list_add_btn(listItems, ListBtn_Grp[i].img_src, str_tmp.c_str());
        
        lv_btn_set_ink_in_time(listItems_btn, 200);
        lv_btn_set_ink_out_time(listItems_btn, 200);
//        lv_obj_set_event_cb(*list_btn, List_EventHandler);
    }
}

void Creat_List()
{
    lv_obj_t * cont = lv_win_get_content(winAbout);
    listItems = lv_list_create(cont, NULL);
    lv_obj_set_size(listItems, lv_obj_get_width_fit(cont) - 4, lv_obj_get_height_fit(cont) - 4);
    lv_obj_align(listItems, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_list_set_edge_flash(listItems, true);
    
    Creat_ListBtn();
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    /*将此页面移到前台*/
    lv_obj_move_foreground(appWindow);
    Creat_WinAbout();
    Creat_List();
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
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
void PageRegister_About(uint8_t pageID)
{
    appWindow = Page_GetAppWindow(pageID);
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
