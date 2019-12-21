#include "SysConfig.h"
#include "TasksManage.h"
#include "CommonMacro.h"

#define TASK_MAX_NUM 10

static uint8_t TaskTotalNum = 0;

typedef struct{
    TaskHandle_t* Handle;
    volatile uint32_t FreeStack; 
}StackCheck_TypeDef;

StackCheck_TypeDef TaskStack_Grp[TASK_MAX_NUM];

bool xTaskAddStackCheckGroup(TaskHandle_t* handle)
{
    if(TaskTotalNum < TASK_MAX_NUM)
    {
        TaskStack_Grp[TaskTotalNum].Handle = handle;
        TaskTotalNum++;
        return true;
    }
    return false;
}

TaskHandle_t xTaskGetIDHandle(uint8_t id)
{
    if(id < TaskTotalNum)
    {
        return *(TaskStack_Grp[id].Handle);
    }
    return 0;
}

void Task_FreeStackMonitor(TimerHandle_t xTimer)
{
    for(uint8_t i = 0; i < TaskTotalNum; i++)
    {
        TaskStack_Grp[i].FreeStack = uxTaskGetFreeStackByte(*(TaskStack_Grp[i].Handle));
    }
}
