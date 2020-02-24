#ifndef __RCX_HANDSHAKE_H
#define __RCX_HANDSHAKE_H

#include "stdint.h"
#include "NRF/NRF.h"

namespace RCX
{
namespace Handshake
{

/*��������*/
typedef enum
{
    CMD_None,
    CMD_AgreeConnect,    /*ͬ������*/
    CMD_AttachConnect,   /*������*/
    CMD_Disconnect,      /*�Ͽ�����*/
    CMD_ReqConnect,      /*��������*/
    CMD_MAX
} CMD_Type;

/*����״̬��*/
typedef enum
{
    State_Idle,        /*����*/
    State_Prepare,     /*׼��*/
    State_Search,      /*����*/
    State_ReqConnect,  /*��������*/
    State_ReConnect,   /*��������*/
    State_Connected,   /*���ӳɹ�*/
    State_MAX
} State_Type;

/*�㲥ͷ*/
typedef enum
{
    BroadcastHead_MasterWait   = (uint16_t)0x55AA,
    BroadcastHead_SlaveWait    = (uint16_t)0xAA55,
    BroadcastHead_MasterAttach = (uint16_t)0xF0F0,
    BroadcastHead_SlaveAttach  = (uint16_t)0x0F0F
}BroadcastHead_Type;

/*ǿ�Ƶ��ֽڶ���*/
#pragma pack (1)

/*�������ݰ���ʽ 32 Bytes*/
typedef struct
{
    /*�㲥֡ͷ 2 Bytes*/
    uint16_t BroadcastHead;
    
    /*ID�� 1 Byte*/
    uint8_t ID;
    
    /*�豸���� 10 Bytes*/
    char Description[10];
    
    /*���ƶ��� 1 Byte*/
    uint8_t Type;

    /*���⹦��ʹ�� 1 Byte*/
    struct
    {
        /*�ش�ʹ��*/
        bool Passback : 1;
        /*��Ƶʹ��*/
        bool FHSS     : 1;
        /*���� 6 bits*/
        uint8_t RESERVED : 6;
    } EnableFunction;

    /*���� 1 Byte*/
    uint8_t CMD;/*CMD_Type*/

    /*ͨ���ٶ� 1 Byte*/
    uint8_t Speed;
    
    /*ͨ�ŵ�ַ 5 Bytes*/
    uint8_t Address[5];
    
    /*��Ƶ�� 10 Bytes*/
    uint8_t FerqList[10];
} Pack_t;

#pragma pack ()

}
}

#endif
