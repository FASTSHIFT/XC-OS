#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "Module.h"
#include "TasksManage.h"

/*状态栏*/
lv_obj_t * barStatus;

static lv_obj_t * labelSignal;
static lv_obj_t * labelTime;
static lv_obj_t * labelCPUusage;
static lv_obj_t * labelBattUsage;

/*导航栏*/
lv_obj_t * barNavigation;

lv_obj_t * btnMenu;
lv_obj_t * btnHome;
lv_obj_t * btnBack;

lv_coord_t BarStatus_GetHeight()
{
    return lv_obj_get_height(barStatus);
}

lv_coord_t BarNavigation_GetHeight()
{
    return lv_obj_get_height(barNavigation);
}

static void Task_UpdateStatusBar(lv_task_t * task)
{
    /*电池电量显示*/
    float battCurrent, battVoltageOc;
    Power_GetInfo(&battCurrent, NULL, &battVoltageOc);
    uint8_t batUsage = map(battVoltageOc, 2600, 4200, 0, 100);
    if(batUsage > 100)batUsage = 100;
    bool Is_BattCharging = (battCurrent > 0 && batUsage < 100) ? true : false;

    const char * battSymbol[] =
    {
        LV_SYMBOL_BATTERY_EMPTY,
        LV_SYMBOL_BATTERY_1,
        LV_SYMBOL_BATTERY_2,
        LV_SYMBOL_BATTERY_3,
        LV_SYMBOL_BATTERY_FULL
    };
    int symIndex = map(
            battVoltageOc, 
            XC_BATTERY_VOLTAGE_MIN, XC_BATTERY_VOLTAGE_MAX, 
            0, __Sizeof(battSymbol));
    if(Is_BattCharging)
    {
        static uint8_t usage = 0;
        usage++;
        usage %= (symIndex + 1);
        symIndex = usage;
    }

    __LimitValue(symIndex, 0, __Sizeof(battSymbol) - 1);
    lv_label_set_text_fmt(labelBattUsage, "%d%s", batUsage, battSymbol[symIndex]);

    /*CPU占用显示*/
    lv_label_set_text_fmt(labelCPUusage, LV_SYMBOL_LOOP"%d%%", FreeRTOS_GetCPUUsage());

    /*时间显示*/
    uint16_t hh = (millis() / (3600 * 1000)) % 100;
    uint16_t mm = (millis() / (60 * 1000)) % 60;
    uint16_t ss = (millis() / 1000) % 60;
    lv_label_set_text_fmt(labelTime, "%02d:%02d:%02d", hh, mm, ss);
}


static void Creat_StatusBar()
{
    barStatus = lv_cont_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(barStatus, lv_disp_get_hor_res(NULL), 25);
    lv_obj_align(barStatus, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    static lv_style_t styleStatusBar;
    lv_style_copy(&styleStatusBar, &lv_style_plain_color);
    styleStatusBar.body.main_color = LV_COLOR_BLACK;
    styleStatusBar.body.grad_color = LV_COLOR_BLACK;
    styleStatusBar.body.opa = LV_OPA_COVER;
    lv_obj_set_style(barStatus, &styleStatusBar);

    /*电池状态*/
    labelBattUsage = lv_label_create(barStatus, NULL);
    lv_label_set_text(labelBattUsage, "--"LV_SYMBOL_BATTERY_EMPTY);
    lv_obj_align(labelBattUsage, NULL, LV_ALIGN_IN_RIGHT_MID, -2, 0);
    lv_obj_set_auto_realign(labelBattUsage, true);

    /*CPU使用量*/
    labelCPUusage = lv_label_create(barStatus, NULL);
    lv_label_set_text(labelCPUusage, LV_SYMBOL_LOOP"--%");
    lv_obj_align(labelCPUusage, labelBattUsage, LV_ALIGN_OUT_RIGHT_MID, -100, 0);
    lv_obj_set_auto_realign(labelCPUusage, true);

    /*时间*/
    labelTime = lv_label_create(barStatus, NULL);
    lv_label_set_text(labelTime, "00:00:00");
    lv_obj_align(labelTime, barStatus, LV_ALIGN_CENTER, 0, 0);
    
    /*SD卡*/
    labelSignal = lv_label_create(barStatus, NULL);
    lv_label_set_text(labelSignal, lv_fs_is_ready('S') ? LV_SYMBOL_SD_CARD : LV_SYMBOL_WARNING);
    lv_obj_align(labelSignal, barStatus, LV_ALIGN_IN_LEFT_MID, 3, 0);

    lv_task_create(Task_UpdateStatusBar, 500, LV_TASK_PRIO_MID, 0);
}

static void NaviButtonEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED)
    {
        Motor_Vibrate(1, 20);
    }
    if(event == LV_EVENT_CLICKED)
    {
        if(DropDownList_GetShow())
        {
            DropDownList_AnimDown(false);
            return;
        }
        
        if(WinOSState_GetShow())
        {
            WinOSState_SetClose();
            return;
        }

        if(obj == btnHome)
        {
            page.PageStackClear();
            page.PagePush(PAGE_Home);
        }
        if(obj == btnMenu)
        {
            Creat_WinOSState();
        }
    }
    page.PageEventTransmit(event, obj);
}

static void Creat_Buttons(lv_obj_t** btn, const char *text, lv_align_t align)
{
    static lv_style_t btnStyle_Release, btnStyle_Press;
    lv_style_copy(&btnStyle_Release, &lv_style_plain_color);
    btnStyle_Release.body.main_color = LV_COLOR_BLACK;
    btnStyle_Release.body.grad_color = LV_COLOR_BLACK;
    btnStyle_Release.body.opa = LV_OPA_TRANSP;

    lv_style_copy(&btnStyle_Press, &lv_style_plain_color);
    btnStyle_Press.body.main_color = LV_COLOR_WHITE;
    btnStyle_Press.body.grad_color = LV_COLOR_WHITE;
    btnStyle_Press.body.opa = LV_OPA_50;

    *btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(*btn, NaviButtonEvent_Handler);
    lv_obj_set_width(*btn, lv_disp_get_hor_res(NULL) / 3);
    lv_obj_align(*btn, barNavigation, align, 0, 0);

    lv_btn_set_style(*btn, LV_BTN_STYLE_REL, &btnStyle_Release);
    lv_btn_set_style(*btn, LV_BTN_STYLE_PR, &btnStyle_Press);

    lv_obj_t * sym = lv_label_create(*btn, NULL);
    lv_label_set_text(sym, text);

    lv_btn_set_ink_in_time(*btn, 200);
    lv_btn_set_ink_out_time(*btn, 200);

    lv_obj_set_protect(*btn, LV_PROTECT_PRESS_LOST);
}

static void Creat_NavigationBar()
{
    static lv_style_t styleNavigationBar;
    lv_style_copy(&styleNavigationBar, &lv_style_plain_color);
    styleNavigationBar.body.main_color = LV_COLOR_BLACK;
    styleNavigationBar.body.grad_color = LV_COLOR_BLACK;
    styleNavigationBar.body.opa = LV_OPA_COVER;

    barNavigation = lv_cont_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_width(barNavigation, lv_disp_get_hor_res(NULL));
    lv_obj_align(barNavigation, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_set_style(barNavigation, &styleNavigationBar);

    Creat_Buttons(&btnMenu, LV_SYMBOL_STOP, LV_ALIGN_IN_LEFT_MID);
    Creat_Buttons(&btnHome, LV_SYMBOL_HOME, LV_ALIGN_CENTER);
    Creat_Buttons(&btnBack, LV_SYMBOL_LEFT, LV_ALIGN_IN_RIGHT_MID);
}

void Init_Bar()
{
    Creat_StatusBar();
    Creat_NavigationBar();
    Creat_AppWindow();
    Creat_DropDownList();
}
