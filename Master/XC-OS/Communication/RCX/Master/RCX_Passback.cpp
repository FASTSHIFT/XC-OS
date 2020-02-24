#include "RCX.h"

/*ǿ�Ƶ��ֽڶ���*/
#pragma pack (1)

typedef struct
{
    uint16_t HeadCode;
    uint8_t Type;
    uint8_t ID;
    float Channel[7];
} Protocol_Passback_xChannel_t;

typedef struct
{
    uint8_t X;
    uint8_t Y;
    uint8_t TextSize;
    uint16_t TextColor;
    uint16_t BackColor;
    char Text[25];
} Protocol_Passback_CommonDisplay_t;

#pragma pack ()

/*�Զ���ش����ݰ��ص�*/
static const uint8_t CallbackGrp_NUM = 10;
static RCX::CustomDataPack_Callback_t UserCustomPassBack_Group[CallbackGrp_NUM] = {0};

/**
  * @brief  �����Զ���ش����ݰ��ص�
  * @param  ��
  * @retval ��
  */
bool RCX::PassBackAddCustom(CustomDataPack_Callback_t callback)
{
    static uint8_t regCnt = 0;

    /*Ѱ���Ƿ����ظ�����*/
    for(uint8_t i = 0; i < CallbackGrp_NUM; i++)
    {
        if(callback == UserCustomPassBack_Group[i])
            return true;
    }

    /*���µĿ�λ���뺯����ַ*/
    if(regCnt < CallbackGrp_NUM)
    {
        UserCustomPassBack_Group[regCnt] = callback;
        regCnt++;
        return true;
    }
    else
        return false;
}

void RCX::PassbackProcess(uint8_t* rxbuff)
{
    /*����ڶ�λΪ0˵��������ͨ�ı��ش�����*/
    bool isTextPassback = (rxbuff[1] == 0) ? false : true;
    if(isTextPassback)
    {
    }
    else
    {
        /*�����û��Զ���ش����ݽ�������*/
        for(int i = 0; i < CallbackGrp_NUM; i++)
        {
            if(UserCustomPassBack_Group[i])
                UserCustomPassBack_Group[i](rxbuff);
            else
                break;
        }
    }
}
