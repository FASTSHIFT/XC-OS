#include "FileGroup.h"
#include "TasksManage.h"

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    Serial3.begin(115200);

    Wire.begin();
    
    /*Task Create*/
    xTaskReg(Task_Dispaly,      KByteToWord(5),    2, &TaskHandle_Display);
    xTaskReg(Task_WavPlayer,    KByteToWord(1),    0, &TaskHandle_WavPlayer);
    xTaskReg(Task_PageRun,      KByteToWord(2),    1, &TaskHandle_PageRun);
    xTaskReg(Task_LuaScript,    KByteToWord(2),    0, &TaskHandle_LuaScript);
//  xTaskReg(Task_TransferData, KByteToWord(0.5f), 0, &TaskHandle_TransferData);
    
    /*Timer Create*/
    xTimerStartSafe(xTimerReg(Task_ReadBattInfo, 500));
    xTimerStartSafe(xTimerReg(Task_FreeStackMonitor, 1000));
    
    TimerHandle_Motor = xTimerReg(Task_MotorRunning, 10);
    xTimerStartSafe(TimerHandle_Motor);
    
//  TimerHandle_IMU_Claculate = xTimerReg(Task_IMU_Claculate, 500);
//  xTimerStartSafe(TimerHandle_IMU_Claculate);

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
    Delay_Init();
    setup();
    for(;;)loop();
}
