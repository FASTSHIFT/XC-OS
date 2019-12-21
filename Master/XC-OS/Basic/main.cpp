#include "FileGroup.h"
#include "TasksManage.h"
#include "cm_backtrace.h"

void setup()
{
    cm_backtrace_init("XC-OS", "v1.0", "v1.0");
    Serial.begin(115200);
    Serial.printf("XC-OS(%s %s Build) init...\r\n", __DATE__, __TIME__);

    Serial2.begin(115200);
    Serial3.begin(115200);

    Wire.begin();
    
    /*Task Create*/
    xTaskReg(Task_Dispaly,      KByteToWord(5),    2, &TaskHandle_Display);
    xTaskReg(Task_WavPlayer,    KByteToWord(1),    0, &TaskHandle_WavPlayer);
    xTaskReg(Task_PageRun,      KByteToWord(3),    1, &TaskHandle_PageRun);
//    xTaskReg(Task_TransferData, KByteToWord(0.5f), 0, &TaskHandle_TransferData);
#if( XC_USE_LUA == 1)
    xTaskReg(Task_LuaScript,    KByteToWord(2),    0, &TaskHandle_LuaScript);
#endif
    
    /*Timer Create*/
    xTimerStartSafe(xTimerReg(Task_ReadBattInfo, 500));
//    xTimerStartSafe(xTimerReg(Task_FreeStackMonitor, 1000));
    
    TimerHandle_Motor = xTimerReg(Task_MotorRunning, 10);
    xTimerStartSafe(TimerHandle_Motor);
    
//  TimerHandle_IMU_Claculate = xTimerReg(Task_IMU_Claculate, 500);
//  xTimerStartSafe(TimerHandle_IMU_Claculate);

    Serial.println("OS Running...");
    Delay_Init();
    vTaskStartScheduler();
}

void loop()
{
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    setup();
    for(;;)loop();
}
