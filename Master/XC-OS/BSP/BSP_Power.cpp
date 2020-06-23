#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "IP5108/IP5108.h"
#include "BSP.h"

TimerHandle_t TimerHandle_Charger;

static IP5108 charger;

static float BattCurrent, BattVoltage, BattVoltageOc;

void Power_GetInfo(float* battCurrent,float* battVoltage,float* battVoltageOc)
{
    if(battCurrent)
        *battCurrent = BattCurrent;
    
    if(battVoltage)
        *battVoltage = BattVoltage;
    
    if(battVoltageOc)
        *battVoltageOc = BattVoltageOc;
}

float Power_GetBattUsage()
{
    float batUsage = __Map(BattVoltageOc, 2600, 4200, 0.0f, 100.0f);
    if(batUsage > 100)
        batUsage = 100;
    return batUsage;
}

void Task_BattInfoUpdate(TimerHandle_t xTimer)
{
    __ExecuteOnce((
        charger.begin(),
        pinMode(CHG_KEY_Pin, INPUT)
    ));
    
    static uint8_t press_cnt = 0;
    digitalRead(CHG_KEY_Pin) == LOW ? press_cnt++ : press_cnt = 0;
    if(press_cnt > 1)
    {
        __ExecuteOnce(Power_Shutdown());
    }
    
    BattCurrent = charger.getBattCurrent();
    BattVoltage = charger.getBattVoltage();
    BattVoltageOc = charger.getBattOcVoltage();
    //Serial.printf("Batt U = %0.2fmV, I = %0.2fmA, ", charger.getBattVoltage(), charger.getBattCurrent());
    //Serial.printf("Oc U = %0.2fmV\r\n", charger.getBattOcVoltage());
}

void Power_Shutdown()
{
    Backlight_SetGradual(0, 200);
    pinMode(CHG_KEY_Pin, OUTPUT_OPEN_DRAIN);
    digitalWrite(CHG_KEY_Pin, LOW);
    Motor_Vibrate(0.5f, 100);
}
