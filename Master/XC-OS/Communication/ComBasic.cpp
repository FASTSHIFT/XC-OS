#include "FileGroup.h"
#include "TasksManage.h"
#include "ComPrivate.h"

TaskHandle_t TaskHandle_Commmunicate;

/*ʧ����ʱ500ms*/
#define ConnectLost_TimeOut 500

/*ʵ����NRF����*/
NRF_Basic nrf(
    NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, 
    NRF_CE_Pin, NRF_CSN_Pin
);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

NRF_TRM  nrfTRM(&nrf);
NRF_FHSS nrfFHSS(&nrf);

/*NRF�������ݻ�����*/
uint8_t NRF_TxBuff[32];

/*NRF�������ݻ�����*/
uint8_t NRF_RxBuff[32];

/*NRF��ַ�����*/
uint8_t NRF_AddressConfig[NRF_AddressNum * 5] =
{
    45, 23, 78, 19, 61, //ADDR:0
    75, 75, 75, 75, 75, //ADDR:1
    12, 34, 56, 78, 90, //ADDR:2
    12, 90, 17, 44, 55, //ADDR:3
    10, 93, 70, 72, 31, //ADDR:4
    0//ADDR: Handshake
};

/*NRF�ź�ǿ��ָʾ*/
int16_t NRF_SignalStrength = 0;

/*NRF�������ñ�*/
NRF_Config_TypeDef NRF_Cfg = {0, 0, 40};

/*��������ʹ��*/
bool State_RF = OFF;

/*�ش�ʹ��*/
bool State_PassBack = ON;

/*ͨ�ûش�ģʽʹ��*/
bool Enable_CommonPassBack = true;

/*�����ش����ݰ�*/
bool Lock_CommonPassback = false;

/*����ʹ��*/
bool State_Handshake = ON;

/*��Ƶʹ��*/
bool State_FHSS = OFF;

/**
  * @brief  NRF��ʼ��
  * @param  ��
  * @retval true�ɹ� falseʧ��
  */
bool Init_NRF()
{
    /*Ĭ�ϳ�ʼ��*/
    nrf.Init();

    /*����ģʽ*/
    nrf.TX_Mode();
    
    /*���õ�ַ*/
    nrf.SetAddress((uint8_t*)NRF_AddressConfig + NRF_Cfg.Address * 5);

    /*����Ƶ��*/
    nrf.SetFreqency(NRF_Cfg.Freq);

    /*��������*/
    nrf.SetSpeed(NRF_Cfg.Speed);

    /*���ݰ�����*/
    nrf.SetPayloadWidth(sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    
    /*����Ƶ*/
    nrf.SetRF_Enable(true);
    
    /*���¼Ĵ���״̬*/
    nrf.UpdateRegs();

    /*�����������*/
    return nrf.IsDetect();
}

/********************** Э��� **********************/
/*������*/
Protocol_Common_t CTRL;

/*����ת��������*/
Protocol_CarDS_t CarDS;

/*���ת��������*/
Protocol_CarSS_t CarSS;

/*ͨ�ö���*/
Protocol_Common_t COMMON_X;

/*��ͨ�����������ݻش�*/
Protocol_Passback_xChannel_t xChannel_Passback;

/*ͨ�����ı����ݻش�*/
Protocol_Passback_CommonDisplay_t Common_Passback;

/*ͨ��״ָ̬ʾ*/
ConnectState_t ConnectState;

/**
  * @brief  ���ݰ�����
  * @param  ��
  * @retval ��
  */
void LoadDataPack()
{
    float Limit_L = CTRL.KnobLimit.L / (float)CtrlOutput_MaxValue;//���޷�ӳ��0.0~1.0
    float Limit_R = CTRL.KnobLimit.R / (float)CtrlOutput_MaxValue;//���޷�ӳ��0.0~1.0

    CTRL.Info.DataPackage_Num++;//���ݰ������Լ�

    /*Ϊ��ǰ�����ƵĶ���������ݰ�*/

    if(CTRL.Info.CtrlObject == X_COMMON)
    {
        COMMON_X = CTRL;
    }
    else if(CTRL.Info.CtrlObject == CAR_DifferentalSteering)
    {
        CarDS.Info = CTRL.Info;
        CarDS.Key = CTRL.Key;
        CarDS.KnobLimit = CTRL.KnobLimit;
        CarDS.KnobCab = CTRL.KnobCab;

        switch(CTRL.Info.CtrlMode)
        {
        case CtrlMode_Tank:
            CarDS.MotorSpeed.L = CTRL.Left.Y * Limit_L;
            CarDS.MotorSpeed.R = CTRL.Right.Y * Limit_R;
            break;

        case CtrlMode_Race:
            CarDS.MotorSpeed.L = CTRL.Left.Y * Limit_L + CTRL.Right.X * Limit_R;
            CarDS.MotorSpeed.R = CTRL.Left.Y * Limit_L - CTRL.Right.X * Limit_R;
            break;

        case CtrlMode_Expert:
            CarDS.MotorSpeed.L = CTRL.Left.Y * Limit_L + CTRL.Left.X * Limit_L * 0.5f;
            CarDS.MotorSpeed.R = CTRL.Left.Y * Limit_L - CTRL.Left.X * Limit_L * 0.5f;
            break;
        }
        __LimitValue(CarDS.MotorSpeed.L, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        __LimitValue(CarDS.MotorSpeed.R, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
    else if(CTRL.Info.CtrlObject == CAR_ServoSteering)
    {
        CarSS.Info = CTRL.Info;
        CarSS.Key = CTRL.Key;
        CarSS.KnobLimit = CTRL.KnobLimit;
        CarSS.KnobCab = CTRL.KnobCab;

        CarSS.MotorSpeed = CTRL.Left.Y;
        CarSS.SteerAngle = CTRL.Right.X;

        __LimitValue(CarSS.MotorSpeed, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        __LimitValue(CarSS.SteerAngle, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
}

/*�Զ��巢�����ݰ��ص�*/
static CustomDataPack_Callback_t LoadUserCustomDataPack = 0;

/*�Զ���ش����ݰ��ص�*/
static CustomDataPack_Callback_t LoadUserCustomPassBack_Group[10] = {0};

/**
  * @brief  �����Զ��巢�����ݰ��ص�
  * @param  ��
  * @retval ��
  */
void SetUserCustomDataPack_Callback(CustomDataPack_Callback_t callback)
{
    LoadUserCustomDataPack = callback;
}

/**
  * @brief  �����Զ���ش����ݰ��ص�
  * @param  ��
  * @retval ��
  */
bool AddUserCustomPassBack_Callback(CustomDataPack_Callback_t callback)
{
    static uint8_t regCnt = 0;

    /*Ѱ���Ƿ����ظ�����*/
    for(uint8_t i = 0; i < __Sizeof(LoadUserCustomPassBack_Group); i++)
    {
        if(callback == LoadUserCustomPassBack_Group[i])
            return true;
    }

    /*���µĿ�λ���뺯����ַ*/
    if(regCnt < __Sizeof(LoadUserCustomPassBack_Group))
    {
        LoadUserCustomPassBack_Group[regCnt] = callback;
        regCnt++;
        return true;
    }
    else
        return false;
}

/**
  * @brief  ���ݷ�������
  * @param  ��
  * @retval ��
  */
static void CommmunicateProcess()
{
    LoadDataPack();//������ݰ�

    /*���ݰ�����NRF���ͻ�����*/
    switch(CTRL.Info.CtrlObject)
    {
    case X_COMMON:
        __TypeExplain(Protocol_Common_t, NRF_TxBuff) = COMMON_X;
        break;

    case CAR_DifferentalSteering:
        __TypeExplain(Protocol_CarDS_t, NRF_TxBuff) = CarDS;
        break;

    case CAR_ServoSteering:
        __TypeExplain(Protocol_CarSS_t, NRF_TxBuff) = CarSS;
        break;
    default:
        if(LoadUserCustomDataPack)
            LoadUserCustomDataPack(NRF_TxBuff);
        break;
    }

    if(ConnectState.Pattern == Pattern_NRF)
    {
        if(State_FHSS)
        {
            nrfFHSS.TxProcess(NRF_TxBuff, NRF_RxBuff);
        }
        /*�Ƿ����ش�*/
        else if(State_PassBack)
        {
            /*NRF�շ�����*/
            nrfTRM.TranRecv(NRF_TxBuff, NRF_RxBuff);

            /*����ڶ�λΪ0˵�����û��Զ���ش�����*/
            if(NRF_RxBuff[1] == 0)
            {
                Enable_CommonPassBack = false;

                /*�����û��Զ���ش����ݽ�������*/
                for(uint8_t i = 0; i < __Sizeof(LoadUserCustomPassBack_Group); i++)
                {
                    if(LoadUserCustomPassBack_Group[i])
                        LoadUserCustomPassBack_Group[i](NRF_RxBuff);
                    else
                        break;
                }
            }
            else
            {
                Enable_CommonPassBack = true;

                /*�Ƿ������ش�����*/
                if(!Lock_CommonPassback)
                    Common_Passback = __TypeExplain(Protocol_Passback_CommonDisplay_t, NRF_RxBuff);
            }
        }
        else
        {
            if(nrf.GetRF_State() != nrf.State_TX)
            {
                nrf.TX_Mode();
            }
            nrf.TranCheck();
            nrf.Tran(NRF_TxBuff);//NRF��������
        }
    }
}

void NRF_Delay_Callback(uint32_t ms)
{
    vTaskDelay(ms);
}

void CommmunicateRF_Enable(bool en)
{
    State_RF = en;
    if(en)
    {
        xTaskNotifyGive(TaskHandle_Commmunicate);
    }
}

/**
  * @brief  ���ݷ�������
  * @param  ��
  * @retval ��
  */
void Task_Commmunicate(void *pvParameters)
{
    State_RF = Init_NRF();
    
    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        while(State_RF)
        {
            CommmunicateProcess();
            vTaskDelay(10);
        }
    }
}
