#ifndef __TASKSMANAGE_H
#define __TASKSMANAGE_H

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"

#include "Source/cpu_utils.h"

/*Ext Function*/
#define DEBUG_PRINTF(format, ...) Serial.printf(format, ##__VA_ARGS__)

#define KByteToWord(kb) (kb * 1024 / 4)
#define uxTaskGetFreeStackByte(TaskHandle_x) (uxTaskGetStackHighWaterMark(TaskHandle_x)*4)


#define xTaskReg(func,stack,priority,handle) \
do{\
    xTaskCreate(func,#func,stack,NULL,priority,handle);\
    DEBUG_PRINTF("Creat: task:%s, stack:%d, priority:%d\r\n",#func,stack,priority);\
}while(0)

#define xTimerReg(func,time)\
do{\
    TimerHandle_t xTimer = xTimerCreate(#func,time,pdTRUE,0,func);\
    if(xTimer)xTimerStart(xTimer,0);\
    DEBUG_PRINTF("Creat: timer:%s, time:%d\r\n",#func,time);\
}while(0)

/*Task Functions*/
void Task_Dispaly(void *pvParameters);
void Task_TransferData(void *pvParameters);
void Task_LuaScript(void *pvParameters);
void Task_WavPlayer(void *pvParameters);
void Task_PageRun(void *pvParameters);

/*Timer Functions*/
void Task_MotorRunning(TimerHandle_t xTimer);
void Task_ReadBattInfo(TimerHandle_t xTimer);
void Task_IMU_Claculate(TimerHandle_t xTimer);

/*TaskHandle*/
extern TaskHandle_t TaskHandle_LuaScript;
extern TaskHandle_t TaskHandle_WavPlayer;
extern TaskHandle_t TaskHandle_Display;
extern TaskHandle_t TaskHandle_PageRun;
extern TaskHandle_t TaskHandle_TransferData;

/*TimerHandle*/

/*QueueHandle*/
extern QueueHandle_t SemHandle_FileSystem;

#endif
