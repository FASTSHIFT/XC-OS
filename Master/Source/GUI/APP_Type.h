#ifndef __APP_TYPE_H
#define __APP_TYPE_H

#include "lvgl.h"
#include "lv_conf.h"

/*Types*/
typedef enum{
    TYPE_None,
    TYPE_PageJump,
    TYPE_FuncCall,
}Func_Type;

/*���APPͼ��ͱ�ǩ������*/
typedef struct
{
    /*�ֶ�*/
    const void* img_dsc;        //ͼƬ
    const char* lable_text;     //��ǩ�ı�
    Func_Type type;             //��ת����
    int param;                  //��������
    
    /*�Զ�*/
    lv_obj_t* cont;             //��ǩ����
    lv_obj_t* imgbtn;           //ͼ�갴ť�ؼ�
    lv_obj_t* label;            //��ǩ�ؼ�
} APP_TypeDef;

void Creat_APP(APP_TypeDef &app, lv_obj_t * parent, uint8_t index, lv_event_cb_t imgbtn_event_handler);

#endif
