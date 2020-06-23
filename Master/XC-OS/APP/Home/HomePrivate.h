#ifndef __HOMEPRIVATE_H
#define __HOMEPRIVATE_H

#include "Basic/CommonMacro.h"
#include "GUI/DisplayPrivate.h"
#include "GUI/Page/APP_Type.h"

/*¶¯»­Ê±¼ä*/
#define AnimCloseTime_Default 300
void Home_AppAnim(
    bool close = true, 
    lv_coord_t x = 0, 
    lv_coord_t y = 0,
    uint16_t time = AnimCloseTime_Default
);
    
lv_obj_t * Home_GetAppWindow();
void Home_CreatAppIcon(APP_TypeDef &app, lv_obj_t * parent, lv_event_cb_t imgbtn_event_handler);
void Home_AppInit(lv_obj_t * parent);
    
#endif
