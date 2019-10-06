#ifndef __IP5018_H
#define __IP5018_H

#include "Wire.h"

#define IP5018_DEFATLT_ADDRESS 0x75

class IP5108{
public :
    IP5108();
    void begin(uint8_t addr = IP5018_DEFATLT_ADDRESS);

    typedef enum{
        SYS_CTL0 = 0x01,
        SYS_CTL1 = 0x02,
        SYS_CTL2 = 0x0C,
        SYS_CTL3 = 0x03,
        SYS_CTL4 = 0x04,
        SYS_CTL5 = 0x07,
        Charger_CTL1 = 0x22,
        Charger_CTL2 = 0x24,
        CHG_DIG_CTL4 = 0x26,
        CHG_DIG_CTL5 = 0x25,
        MFP_CTL0 = 0x51,
        MFP_CTL1 = 0x52,
        GPIO_CTL1 = 0x53,
        GPIO_CTL2 = 0x54,
        GPIO_CTL3 = 0x55,
        BATVADC_DAT0 = 0xA2,
        BATVADC_DAT1 = 0xA3,
        BATIADC_DAT0 = 0xA4,
        BATIADC_DAT1 = 0xA5,
        BATOCV_DAT0 = 0xA8,
        BATOCV_DAT1 = 0xA9,
        Reg_READ0 = 0x71,
        Reg_READ1 = 0x72,
        Reg_READ2 = 0x77,
    }REG_t;
    
    typedef enum{
        /*************************** SYS_CTL0 ***************************/
        SYS_CTL0_BIT_FlashLight = 1 << 4,/*�ֵ�Ͳ����Ƿ�ʹ��  1��enable 0��disable*/
        SYS_CTL0_BIT_Light      = 1 << 3,/*Light enable        1��enable 0��disable*/
        SYS_CTL0_BIT_Boost      = 1 << 2,/*Boost enable        1��enable 0��disable*/
        SYS_CTL0_BIT_Charger    = 1 << 1,/*Charger enable      1��enable 0��disable*/
        
        /*************************** SYS_CTL1 ***************************/
        /*���عػ�ʹ�ܣ�0x0c ���趨���عػ���ֵ��
        1:ʹ�� IBATLOW ���عػ�����
        0:�ر� IBATLOW ���عػ�����*/
        SYS_CTL1_BIT_LowLoadOff = 1 << 1,
        
        /*���ز����Զ�����
        1:ʹ��
        0:�ر�*/
        SYS_CTL1_BIT_LoadInsertAutoStartUp = 1 << 0,
        
        /*************************** SYS_CTL2 ***************************/
        /*���عػ�������ֵ�趨
        n * 12mA
        �� BAT ����С���趨��ֵʱ������ 32s �ػ�
        ע�⣺�˵����趨��ֵ��Ҫ���� 100mA*/
        SYS_CTL2_BIT_CurrentThreshold = B00011111,
        
        /*************************** SYS_CTL3 ***************************/
        /*��������ʱ��ѡ��
        00��1S
        01��2S
        10��3S
        11��4S*/
        SYS_CTL3_BIT_LongPressTimeSet = B00000011,
        
        /*1���������ζ̰������ζ̰��� 1s �ڣ�
        �ػ�����ʹ��
        0���������ζ̰������ζ̰��� 1s �ڣ�
        �ػ����ܹر�*/
        SYS_CTL3_BIT_DoubleShortPress = 1 << 5,
        
        /*************************** SYS_CTL4 ***************************/
        /*�ػ�ʱ���趨
        11:64s
        10:32s
        01:16s
        00: 8s*/
        SYS_CTL4_BIT_ShutdownTimeSet = B00000011,
        
        /*VIN �γ��Ƿ��� BOOST
        1������
        0��������*/
        SYS_CTL4_BIT_VIN_PullOutBoost = 1 << 5,
        
        /*************************** SYS_CTL5 ***************************/
        /*NTC ����ʹ��
        0��ʹ��
        1���ر�*/
        SYS_CTL5_BIT_NTC_Function = 1 << 6,
        
        /*�������� WLED �ֵ�Ͳ��ʽѡ��
        0������ 2S
        1���̰����ΰ���*/
        SYS_CTL5_BIT_WLED_Set = 1 << 1,
        
        /*�����ػ���ʽѡ��
        0���̰����ΰ���
        1������ 2S*/
        SYS_CTL5_BIT_KeyShutdownSet = 1 << 0,

        /*************************** Charger_CTL1 ***************************/
        /*���Ƿѹ���趨�����ʱ����� VOUT �ĵ�ѹ��
        11:4.83V
        10:4.73V
        01:4.63V
        00:4.53V
        ע���ڳ���ʱ�� IC ������� VOUT
        �ĵ�ѹ���Զ��������������� VOUT ��
        ��ѹ��������ֵʱ�����������Գ��
        ��磬С���趨ֵʱ���Զ���С������
        ��ά�ִ˵�ѹ��
        ����ͻ�Ҫ��߳�߷�״̬�¿������
        �˼Ӳ���������߳�߷�״̬�����
        �ĸ��ص������� 100mA ʱ�ɰ�Ƿѹ����
        ��Ϊ��ߣ����ȶ��ⲿ���س�硣*/
        Charger_CTL1_BIT_ChargeUndervoltageRingSet = B00110000,
        
        /*************************** Charger_CTL2 ***************************/
        /*BAT �������ѡ��
        11��RESERVED
        10��4.35V ���
        01��4.3V ���
        00: 4.2V ���*/
        Charger_CTL2_BIT_BatteryTypeSet = B00000110,
        
        /*��ѹ����ѹ��ѹ����
        11����ѹ 42mV
        10����ѹ 28mV
        01����ѹ 14mV
        00: ����ѹ
        ע��4.30V/4.35V �����ѹ 14mV��
        4.2V �����ѹ 28mV��
        ����ͻ���Ҫ֧�� 4.4V �ĵ�о��������
        4.35V ��صĻ�����ѡ���ѹ 48mV����
        ���� MCU ��⵽��ص�ѹ���� 4.4V����
        ��С�� 200MA ����Ϊ�ǵ�о�䱥�ˣ�*/
        Charger_CTL2_BIT_ConstantVoltageForcingSet = B01100000,
        
        /*************************** CHG_DIG_CTL4 ***************************/
        /*��������ڲ��Ĵ����趨�����ⲿ Vset PIN �趨ѡ��
        1���ⲿ VSET PIN ����
        0���ڲ��Ĵ�������
        ����Ǹ� bit Ϊ 0����ͨ�� 0x24 �Ĵ����� bit6��5
        ���趨�������*/
        CHG_DIG_CTL4_BIT_BatteryTypeInternalSet = 1 << 6,
        
        /*************************** CHG_DIG_CTL5 ***************************/
        /*����������(����Ϊ��ض˵���)��
        Ibat=b0*0.1+b1*0.2+b2*0.4+b3*0.8+b4*1.6A
        ע��Ĭ��ֵΪ 10111 2.3A ����*/
        CHG_DIG_CTL5_BIT_ChargeCurrentSet = B11111000,
        
        /*************************** MFP_CTL0 ***************************/
        /*LIGHT ����ѡ��
        00��WLED
        01��GPIO2
        10: VREF
        11 : Reserved*/
        MFP_CTL0_BIT_LIGHT_Sel = B00001100,
        
        /*L4 �Ĺ���ѡ��
        00��L4
        01��GPIO1
        10: Reserved
        11: Reserved*/
        MFP_CTL0_BIT_L4_Sel = B00110000,
        
        /*L3�Ĺ���ѡ��
        00��L3
        01��GPIO0
        10: Reserved
        11 : Reserved*/
        MFP_CTL0_BIT_L3_Sel = B11000000,
        
        /*************************** MFP_CTL1 ***************************/
        /*VSET����ѡ��
        00����ص�ѹѡ��PIN
        01��GPIO4
        10: Reserved
        11: Reserved*/
        MFP_CTL1_BIT_VSET_Sel = B00110000,
        
        /*RSET����ѡ��
        00���������ѡ��PIN
        01��GPIO3
        10: Reserved
        11: Reserved*/
        MFP_CTL1_BIT_RSET_Sel = B11000000,
        
        /*************************** GPIO_CTL1 ***************************/
        /*GPIO[4:0]input enable
        0��Disable
        1��Enable*/
        GPIO_CTL1_BIT_GPIO_INEN = B11111000,
        
        /*************************** GPIO_CTL2 ***************************/
        /*GPIO[4:0]output enable
        0��Disable
        1��Enable
        �ڿ��� Output ֮ǰ����Ҫ�Ƚ� data ���*/
        GPIO_CTL2_BIT_GPIO_OUTEN = B11111000,
        
        /*************************** GPIO_CTL3 ***************************/
        /*GPIO[4:0] DATA */
        GPIO_CTL3_BIT_GPIO_DAT = B11111000,
        
        /*************************** BATVADC_DAT0 ***************************/
        /*BATVADC ���ݵĵ� 8bit*/
        BATVADC_DAT0_BIT_BATVADC_Low_8Bit = B11111111,
        
        /*BATVADC ���ݵĸ� 6bit
        VBAT=BATVADC*0.26855mv+2.6V*/
        BATVADC_DAT0_BIT_BATVADC_High_6Bit = B11111100,
        
        /*************************** BATIADC_DAT0 ***************************/
        /*BATIADC ���ݵĵ� 8bit */
        BATIADC_DAT0_BIT_BATVADC_Low_8Bit = B11111111,
        
        /*BATIADC ���ݵĸ� 6bit
        VBAT=BATiADC*0.745985mA*/
        BATIADC_DAT0_BIT_BATVADC_High_6Bit = B11111100,
        
        /*************************** BATOCV_DAT0 ***************************/
        /*BATOCV ���ݵĵ� 8bit*/
        BATOCV_DAT0_BIT_BATOCV_Low_8Bit = B11111111,
        
        /***************************  BATOCV_DAT1 ***************************/
        /*BATOCV ���ݵĸ� 6bit
        OCV=BATOCV*0.26855mv+2.6V*/
        BATOCV_DAT1_BIT_BATOCV_High_6Bit = B11111100,
        
        /***************************  Reg_READ0 ***************************/
        /*���״ָ̬ʾ��־
        000: idle
        001: ������״̬
        010: �������״̬
        011: ��ѹ���״̬
        100: ��ѹֹͣ�����
        101: �䱥״̬
        110: ��糬ʱ״̬*/
        Reg_READ0_BIT_ChargeStatusFlags = B00000111,
        
        /*Chgop
        0��
        1*/
        Reg_READ0_BIT_Chgop = 1 << 4,
        
        /*��������־
        0:
        1: ������*/
        Reg_READ0_BIT_ChargeFinishFlag = 1 << 3,
        
        /*��ѹ��ʱ��־
        0��
        1����ѹ��ʱ*/
        Reg_READ0_BIT_ConstantVoltageTimeoutFlag = 1 << 2,
        
        /*��糬ʱ
        0��
        1����糬ʱ*/
        Reg_READ0_BIT_ChargeTimeoutFlag = 1 << 1,
        
        /*�����糬ʱ
        0��
        1�������糬ʱ*/
        Reg_READ0_BIT_TrickleChargeTimeoutFlag = 1 << 0,
        
        /*************************** Reg_READ1 ***************************/
        /*Light LED �Ƿ��нӵ�
        0���� light LED ��
        1���� light LED ��*/
        Reg_READ1_BIT_HaveLightLED = 1 << 7,
        
        /*���ر�־λ
        0������״̬(����˵������� 75mA)
        1������״̬(����˵���С�� 75mA)*/
        Reg_READ1_BIT_LowLoadFlag = 1 << 6,
        
        /*�����ѹ״̬
        0��VIN ��ѹС�� 5.6V
        1��VIN ��ѹ���� 5.6V�������ѹ״̬*/
        Reg_READ1_BIT_InputOverVoltageFlag = 1 << 5,
        
        /*************************** Reg_READ2 ***************************/
        /*����������־
        0����ǰ����û�а���
        1����ǰ��������*/
        Reg_READ2_BIT_KeyPressFlag = 1 << 3,
        
        /*����������־λ
        0��
        1��onoff ��������*/
        Reg_READ2_BIT_KeyLongPressFlag = 1 << 1,
        
        /*�����̰���־λ
        0��
        1��onoff �����̰�*/
        Reg_READ2_BIT_KeyClickPressFlag = 1 << 0
        
    }REG_BIT_t;

    uint8_t readReg(REG_t reg);
    bool readRegBit(REG_t reg, REG_BIT_t bit);
    void writeReg(REG_t reg, uint8_t val);
    void writeRegBit(REG_t reg, REG_BIT_t bit, bool val);
    
    float getBattVoltage();
    float getBattCurrent();
    float getBattOcVoltage();

private:
    uint8_t Address;
};

#endif
