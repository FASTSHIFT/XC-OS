#include "FileGroup.h"
#include "TasksManage.h"
#include "cm_backtrace.h"

void setup()
{
    cm_backtrace_init(XC_OS_NAME, XC_VERSION_HW, XC_VERSION_SW);
    Serial.begin(115200);
    Serial.println("\r\n+===================== XC-OS =======================+\r\n");
    Serial.println("@FreeRTOS Kernel "tskKERNEL_VERSION_NUMBER);
    Serial.print(XC_OS_NAME"("__DATE__" "__TIME__" Build) init...\r\n");

    Serial2.begin(115200);
    Serial3.begin(115200);
    Wire.begin();
    
    /*Task Create*/
    xTaskReg(Task_Dispaly,      KByteToWord(5),    2, &TaskHandle_Display);
    xTaskReg(Task_WavPlayer,    KByteToWord(1),    0, &TaskHandle_WavPlayer);
    xTaskReg(Task_PageRun,      KByteToWord(3),    1, &TaskHandle_PageRun);
    xTaskReg(Task_Commmunicate, KByteToWord(0.5f), 0, &TaskHandle_Commmunicate);
#if( XC_USE_LUA == 1 )
    xTaskReg(Task_LuaScript,    KByteToWord(3),    0, &TaskHandle_LuaScript);
#endif
    
    /*Timer Create*/
    xTimerReg(Task_ReadBattInfo, 500);
    xTimerReg(Task_MotorRunning, 10);

    Serial.println("OS start scheduler...");
    Serial.print("\r\n+===================================================+\r\n");
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
