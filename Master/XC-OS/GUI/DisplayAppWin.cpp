#include "DisplayPrivate.h"

extern lv_obj_t * barStatus;

typedef struct{
    lv_obj_t * cont;
    lv_style_t style;
}AppWindow_TypeDef;

AppWindow_TypeDef appWindow_Grp[PAGE_MAX];

lv_obj_t * Page_GetAppWindow(uint8_t pageID)
{
    lv_obj_t * win = NULL;
    if(pageID < PAGE_MAX)
    {
        win = appWindow_Grp[pageID].cont;
    }
    return win;
}

void Creat_AppWindow()
{
    for(uint8_t i = 0; i < PAGE_MAX; i++)
    {
        appWindow_Grp[i].cont = lv_cont_create(lv_scr_act(), NULL);

        lv_obj_set_size(appWindow_Grp[i].cont, APP_WIN_WIDTH, APP_WIN_HEIGHT);
        lv_obj_align(appWindow_Grp[i].cont, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        lv_cont_set_fit(appWindow_Grp[i].cont, LV_FIT_NONE);
        
        appWindow_Grp[i].style = lv_style_plain;
        lv_cont_set_style(appWindow_Grp[i].cont, LV_CONT_STYLE_MAIN, &appWindow_Grp[i].style);
    }
}
