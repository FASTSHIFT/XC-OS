#include "DisplayPrivate.h"
#include "TasksManage.h"
#include "Module.h"
#include "CommonMacro.h"

extern lv_obj_t * barStatus;
static lv_obj_t * contDropDown;

static lv_obj_t * sliderBright;

static void Creat_Slider();

void DropDownList_AnimDown(bool down);

bool DropDownList_GetShow()
{
    return (lv_obj_get_y(contDropDown) + lv_obj_get_height(contDropDown) >= 0);
}

static void DropDownList_AnimEndEvent(lv_anim_t * a)
{
#define DropDownList_ExtArea (BarStatus_GetHeight()*2)
    lv_obj_set_ext_click_area(contDropDown, 0, 0, 0, DropDownList_GetShow() ? 0 : DropDownList_ExtArea);
}

void DropDownList_AnimDown(bool down)
{
    static lv_anim_t a;
    a.var = contDropDown;
    a.start = lv_obj_get_y(contDropDown);

    a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
    a.path_cb = lv_anim_path_linear;
    a.ready_cb = DropDownList_AnimEndEvent;
    a.time = 300;
    if(down)
    {
        a.end = 0;
    }
    else
    {
        a.end = -lv_obj_get_height(contDropDown) - 5;
    }

    lv_anim_create(&a);
}

static void DropDownListEvent_Handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED)
    {
        lv_obj_move_foreground(contDropDown);
        lv_slider_set_value(sliderBright, Brightness_GetValue(), LV_ANIM_ON);
    }
    else if(event == LV_EVENT_RELEASED || event == LV_EVENT_DRAG_END)
    {
        lv_coord_t ver = lv_disp_get_ver_res(NULL);
        lv_coord_t y = lv_obj_get_y(contDropDown) + lv_obj_get_height(contDropDown);
        if(y > ver / 2)
        {
            DropDownList_AnimDown(true);
        }
        else
        {
            DropDownList_AnimDown(false);
        }
    }
}

void Creat_DropDownList()
{
    contDropDown = lv_cont_create(lv_scr_act(), NULL);

    static lv_style_t style = *lv_obj_get_style(contDropDown);
    style.body.main_color = LV_COLOR_BLACK;
    style.body.grad_color = LV_COLOR_BLACK;
    style.body.opa = LV_OPA_80;
    lv_cont_set_style(contDropDown, LV_CONT_STYLE_MAIN, &style);

    lv_obj_set_size(contDropDown, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL) - BarNavigation_GetHeight());
    lv_obj_set_drag(contDropDown, true);
    lv_obj_set_drag_dir(contDropDown, LV_DRAG_DIR_VER);
//    lv_obj_set_drag_throw(contDropDown, true);
    lv_obj_set_ext_click_area(contDropDown, 0, 0, 0, DropDownList_ExtArea);
    lv_obj_set_event_cb(contDropDown, DropDownListEvent_Handler);
    lv_obj_align(contDropDown, barStatus, LV_ALIGN_OUT_TOP_MID, 0, -5);

    /*label*/
    lv_obj_t * label = lv_label_create(contDropDown, NULL);
    lv_label_set_text(label, LV_SYMBOL_DOWN);
    lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    
    Creat_Slider();
}

/****************** Bright Ctrl ****************/

//static lv_obj_t * labelBright;

static void SliderBright_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        int value = lv_slider_get_value(obj);
//        lv_label_set_text_fmt(labelBright, "%d%%", value / 10);
        Brightness_SetValue(value);
//        __IntervalExecute(Serial3.printf("B%dE", value), 20);
    }
}

static void Creat_Slider()
{
    /*Create a slider*/
    sliderBright = lv_slider_create(contDropDown, NULL);
    lv_slider_set_range(sliderBright, 5, 1000);

    lv_obj_set_size(sliderBright, APP_WIN_WIDTH - 30, 20);
    lv_obj_align(sliderBright, NULL, LV_ALIGN_IN_TOP_MID, 0, 60);
    lv_obj_set_event_cb(sliderBright, SliderBright_EventHandler);
    
//    labelBright = lv_label_create(contDropDown, NULL);
//    lv_label_set_text_fmt(labelBright, "%d%%", BrightnessGet() / 10);
//    lv_obj_align(labelBright, sliderBright, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
}
