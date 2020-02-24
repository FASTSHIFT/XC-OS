#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "BSP.h"

static bool State_MotorVibrate = true;
static uint32_t MotorStop_TimePoint = 0;
static bool IsMotorRunning = false;

static void Init_Motor()
{
    PWM_Init(Motor_Pin, 1000, 1000);
    analogWrite(Motor_Pin, 0);
}

void Task_MotorRunning(TimerHandle_t xTimer)
{
    __ExecuteOnce(Init_Motor());
    
    if(IsMotorRunning && millis() >= MotorStop_TimePoint)
    {
        analogWrite(Motor_Pin, 0);
        IsMotorRunning = false;
    }
}

void Motor_SetEnable(bool en)
{
    State_MotorVibrate = en;
}

void Motor_Vibrate(float strength, uint32_t time)
{
   if(!State_MotorVibrate)
       return;
   
   __LimitValue(strength, 0.0f, 1.0f);
   
   analogWrite(Motor_Pin, strength * 1000);
   IsMotorRunning = true;
   MotorStop_TimePoint = millis() + time;
}
