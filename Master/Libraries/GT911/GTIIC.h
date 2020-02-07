#ifndef __GT_IIC_H
#define __GT_IIC_H

#include "Arduino.h"
#include "Basic\SysConfig.h"

#define GT_SDA_Pin Touch_SDA_Pin
#define GT_SCL_Pin Touch_SCL_Pin

//IO��������
//#define GT_SDA_IN()  pinMode(GT_SDA_Pin, INPUT)//{GPIOF->CRH&=0XFFFFFF0F;GPIOF->CRH|=8<<4*1;}
//#define GT_SDA_OUT() pinMode(GT_SDA_Pin, OUTPUT)//{GPIOF->CRH&=0XFFFFFF0F;GPIOF->CRH|=3<<4*1;}

//IO��������
#define GT_IIC_SCL_SET    digitalWrite_HIGH(GT_SCL_Pin)//SCL
#define GT_IIC_SCL_CLR    digitalWrite_LOW(GT_SCL_Pin)
#define GT_IIC_SDA_SET    digitalWrite_HIGH(GT_SDA_Pin)//SDA
#define GT_IIC_SDA_CLR    digitalWrite_LOW(GT_SDA_Pin)
#define GT_READ_SDA       digitalRead_FAST(GT_SDA_Pin)//����SDA 

//IIC���в�������
void GT_IIC_Init(void);                 //��ʼ��IIC��IO��
void GT_IIC_Start(void);                //����IIC��ʼ�ź�
void GT_IIC_Stop(void);                 //����IICֹͣ�ź�
void GT_IIC_Send_Byte(uint8_t txd);          //IIC����һ���ֽ�
uint8_t GT_IIC_Read_Byte(uint8_t ack); //IIC��ȡһ���ֽ�
uint8_t GT_IIC_Wait_Ack(void);               //IIC�ȴ�ACK�ź�
void GT_IIC_Ack(void);                  //IIC����ACK�ź�
void GT_IIC_NAck(void);                 //IIC������ACK�ź�

#endif







