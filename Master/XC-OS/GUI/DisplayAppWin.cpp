#include "DisplayPrivate.h"

typedef struct{
    lv_obj_t *cont;
    lv_style_t style;
}AppWindow_TypeDef;

AppWindow_TypeDef appWindow_Grp[PAGE_MAX];

lv_obj_t * AppWindow_PageGet(uint8_t pageID)
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
#define PAGE_HIEGHT (lv_disp_get_ver_res(NULL) - lv_obj_get_height(barStatus) - lv_obj_get_height(barNavigation))
#define PAGE_WIDTH  (lv_disp_get_hor_res(NULL))
    
    for(uint8_t i = 0; i < PAGE_MAX; i++)
    {
        appWindow_Grp[i].cont = lv_cont_create(lv_scr_act(), NULL);

        lv_obj_set_size(appWindow_Grp[i].cont, PAGE_WIDTH, PAGE_HIEGHT);
        lv_obj_align(appWindow_Grp[i].cont, barStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        lv_cont_set_fit(appWindow_Grp[i].cont, LV_FIT_NONE);
        
        appWindow_Grp[i].style = lv_style_transp;
        lv_cont_set_style(appWindow_Grp[i].cont, LV_CONT_STYLE_MAIN, &appWindow_Grp[i].style);
    }
}
