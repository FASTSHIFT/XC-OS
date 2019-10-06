#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "GT911.h"   //����GT911����IC

#define TP_PRES_DOWN 0x80  //����������   
#define TP_CATH_PRES 0x40  //�а���������     
#define CT_MAX_TOUCH  5    //������֧�ֵĵ���,�̶�Ϊ5��

//������������
typedef struct
{
    uint8_t (*init)(void);           //��ʼ��������������
    uint8_t (*scan)(void);               //ɨ�败����.0,��Ļɨ��;1,��������;
    uint16_t x[CT_MAX_TOUCH];        //��ǰ����
    uint16_t y[CT_MAX_TOUCH];        //�����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
    //x[4],y[4]�洢��һ�ΰ���ʱ������.
    uint8_t  sta;                    //�ʵ�״̬
    //b7:����1/�ɿ�0;
    //b6:0,û�а�������;1,�а�������.
    //b5:����
    //b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
} Touch_TypeDef;

extern Touch_TypeDef tp_dev;        //������������touch.c���涨��

uint8_t TP_Init(void);                               //��ʼ��

#endif

















