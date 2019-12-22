#include "FileGroup.h"
#include "TasksManage.h"
#include "IP5108.h"
#include "Module.h"

TimerHandle_t TimerHandle_Charger;

static IP5108 charger;

float BattCurret, BattVoltage, BattVoltageOc;

void Task_ReadBattInfo(TimerHandle_t xTimer)
{
    __ExecuteOnce((
        charger.begin(),
        pinMode(CHG_KEY_Pin, INPUT)
    ));
    
    static uint8_t press_cnt = 0;
    digitalRead(CHG_KEY_Pin) == LOW ? press_cnt++ : press_cnt = 0;
    if(press_cnt > 1)
        Power_Shutdown();
    
    BattCurret = charger.getBattCurrent();
    BattVoltage = charger.getBattVoltage();
    BattVoltageOc = charger.getBattOcVoltage();
    //Serial.printf("Batt U = %0.2fmV, I = %0.2fmA, ", charger.getBattVoltage(), charger.getBattCurrent());
    //Serial.printf("Oc U = %0.2fmV\r\n", charger.getBattOcVoltage());
}

void Power_Shutdown()
{
    Brightness_SetGradual(0, 200);
    pinMode(CHG_KEY_Pin, OUTPUT_OPEN_DRAIN);
    digitalWrite(CHG_KEY_Pin, LOW);   
}
