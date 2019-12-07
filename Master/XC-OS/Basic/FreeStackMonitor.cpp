#include "SysConfig.h"
#include "TasksManage.h"
#include "CommonMacro.h"

typedef struct{
    TaskHandle_t * taskHandle;
    volatile uint32_t freeStackSize; 
}StackCheck_TypeDef;

StackCheck_TypeDef TaskStack_Grp[] = {
    {&TaskHandle_Display  },
    {&TaskHandle_WavPlayer},
    {&TaskHandle_PageRun  },
#if( XC_USE_LUA == 1 )
    {&TaskHandle_LuaScript}
#endif
};

void Task_FreeStackMonitor(TimerHandle_t xTimer)
{
    for(int i = 0; i < __Sizeof(TaskStack_Grp); i++)
    {
        TaskStack_Grp[i].freeStackSize = uxTaskGetFreeStackByte(*(TaskStack_Grp[i].taskHandle));
    }
}
