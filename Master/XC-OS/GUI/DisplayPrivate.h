#ifndef __DISPLAYPRIVATE_H
#define __DISPLAYPRIVATE_H

/*Basic*/
#include "TFT_ILI9488/TFT_ILI9488.h"
#include "GT911/touch.h"

typedef TFT_ILI9488 SCREEN_CLASS;
extern SCREEN_CLASS screen;

/*SCREEN basic*/
#define TEXT_HEIGHT_1   8
#define TEXT_WIDTH_1    6
#define TextSetDefault() screen.setTextColor(screen.White,screen.Black),screen.setTextSize(1)
#define TextMid(x,textnum) (screen.width()*(x)-(TEXT_WIDTH_1*(textnum))/2.0f)
#define TextMidPrint(x,y,text) screen.setCursor(TextMid(x,strlen(text)),screen.height()*(y)-TEXT_HEIGHT_1/2.0f),screen.print(text)
#define ScreenMid_W (screen.width()/2)
#define ScreenMid_H (screen.height()/2)

/*LittleVGL*/
#include "lvgl.h"
#include "lv_conf.h"

void lv_disp_init();
void lv_fsys_init();

/*lvgl扩展函数*/
bool lv_obj_del_safe(lv_obj_t** obj);
void lv_label_set_text_add(lv_obj_t * label, const char * text);
lv_coord_t lv_obj_get_x_center(lv_obj_t * obj);
lv_coord_t lv_obj_get_y_center(lv_obj_t * obj);
void lv_obj_set_color(lv_obj_t * obj, lv_color_t color);
void lv_table_set_align(lv_obj_t * table, lv_label_align_t align);
lv_obj_t * lv_win_get_label(lv_obj_t * win);
void lv_obj_add_anim(
    lv_obj_t * obj, lv_anim_t * a,
    lv_anim_exec_xcb_t exec_cb, 
    int32_t start, int32_t end,
    uint16_t time = 200,
    lv_anim_ready_cb_t ready_cb = NULL,
    lv_anim_path_cb_t path_cb = lv_anim_path_ease_in_out
);

/*Page*/
#include "LightGUI/PageManager/PageManager.h"

typedef enum
{
    /*保留*/
    PAGE_NONE,
    /*用户页面*/
    PAGE_About,
    PAGE_BattInfo,
    PAGE_BvPlayer,
    PAGE_FileExplorer,
    PAGE_Game,
    PAGE_Home,
    PAGE_LuaScript,
    PAGE_RadioCfg,
    PAGE_Settings,
    PAGE_SetDisplay,
    PAGE_SubAPPs,
    PAGE_TextEditor,
    PAGE_USB,
    PAGE_WavPlayer,
    /*保留*/
    PAGE_MAX
} Page_Type;

extern PageManager page;

/*Bar*/
void Init_Bar();
lv_coord_t BarStatus_GetHeight();
lv_obj_t * BarStatus_GetObj();
lv_coord_t BarNavigation_GetHeight();
lv_obj_t * BarNavigation_GetObj();

extern lv_obj_t * btnMenu;
extern lv_obj_t * btnHome;
extern lv_obj_t * btnBack;

/*DropDownList*/
bool DropDownList_GetShow();
void DropDownList_AnimDown(bool down);
void Creat_DropDownList();

/*OSState*/
void Creat_WinOSState();
bool WinOSState_GetShow();
void WinOSState_SetClose();

/*AppWindow*/
void Creat_AppWindow();
lv_obj_t * AppWindow_GetCont(uint8_t pageID);
lv_coord_t AppWindow_GetHeight();
lv_coord_t AppWindow_GetWidth();

#define APP_WIN_HEIGHT AppWindow_GetHeight()
#define APP_WIN_WIDTH  AppWindow_GetWidth()

/*Widget*/
void Preloader_Activate(
    bool isact, 
    lv_obj_t * parent
);
    
void Keyboard_Activate(
    lv_obj_t** kb,
    bool isact, 
    lv_obj_t * parent, 
    lv_obj_t * ta, 
    lv_event_cb_t keyboard_event_cb
);
    
void MessageBox_Activate(
    bool isact, 
    lv_obj_t * parent, 
    lv_obj_t** mbox,
    lv_coord_t w, lv_coord_t h,
    const char *text,
    const char** btns,
    lv_event_cb_t mbox_event_handler
);

#endif
