#ifndef __XC_TYPE_H
#define __XC_TYPE_H

#include "stdint.h"

/*����*/
typedef struct
{
    double Start;
    double End;
} CurveSlope_TypeDef;

/*У׼*/
typedef struct
{
    int16_t Val;
    int16_t Min;
    int16_t Mid;
    int16_t Max;
    bool Reverse;
    CurveSlope_TypeDef Curve;
} Channel_TypeDef;

/*ҡ��*/
typedef struct
{
    Channel_TypeDef X;
    Channel_TypeDef Y;
} Joystick_TypeDef;

/*����*/
typedef struct
{
    int16_t X;
    int16_t Y;
} Axis_TypeDef;

/*����*/
typedef struct
{
    int16_t L;
    int16_t R;
} Direction_TypeDef;

/*ң����״̬*/
typedef struct
{

    /*ҡ��*/
    Joystick_TypeDef JS_L;
    Joystick_TypeDef JS_R;

    /*����*/
    union
    {
        struct
        {
            uint8_t BT_UP : 1;
            uint8_t BT_DOWN : 1;
            uint8_t BT_BACK : 1;
            uint8_t BT_OK : 1;
            uint8_t BT_L1 : 1;
            uint8_t BT_L2 : 1;
            uint8_t BT_R1 : 1;
            uint8_t BT_R2 : 1;
        } Bit;
        uint8_t Value;
    } Key;

    /*�޷�*/
    Direction_TypeDef KnobLimit;

    /*����ʹ��*/
    struct
    {
        bool PassBack;
        bool Handshake;
        bool FHSS;
        bool Sound;
        bool Vibrate;
        bool IMU;
        bool LuaScript;
        bool CPUInfo;
        bool IdleWarn;
    } State;

    /*�����Ϣ*/
    struct
    {
        float Usage;
        float Voltage;
    } Battery;

    /*CPU��Ϣ*/
    struct
    {
        float Usage;
        float Temperature;
        bool  Enable;
    } CPU;

    /*����*/
    struct
    {
        uint8_t ConnectObject;
        bool Enable;
    } Bluetooth;
} CTRL_TypeDef;

#endif
