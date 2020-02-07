#include "DisplayPrivate.h"
#include "Basic/CommonMacro.h"
#include "Basic/TasksManage.h"

static lv_obj_t * winOSState;
static lv_obj_t * tableOSState;
static lv_task_t * taskOSState;

#define TASK_NUM_MAX 10

TaskStatus_t TaskStatus_Grp[TASK_NUM_MAX];
uint32_t TaskLastRunTime[TASK_NUM_MAX];
UBaseType_t TaskStatusNum = 0;
uint32_t RunTimeCounter = 0;

#define TASK_INFO_COL 5
#define TASK_INFO_ROW (TaskStatusNum+1)

static void TimerForRuntime_Callback()
{
    RunTimeCounter++;
}

static void AppConfigureTimerForRuntimeStats()
{
    Timer_SetInterrupt(XC_TIM_RUNTIME, 10, TimerForRuntime_Callback);
    TIM_Cmd(XC_TIM_RUNTIME, ENABLE);
}

static void Task_UpdateTaskStatus(lv_task_t * task)
{

    if(!winOSState)
    {
        lv_task_del(task);
        return;
    }

    uint32_t totalRunTime;
    static uint32_t lastRunTime, currentTime;;

    uxTaskGetSystemState(TaskStatus_Grp, TaskStatusNum, &totalRunTime);

    currentTime = totalRunTime - lastRunTime;
    lastRunTime = totalRunTime;

    /*"TASK", "CPU", "STA", "PRIO", "STK"*/
    for(int i = 0; i < TaskStatusNum; i++)
    {
        lv_table_set_cell_align(tableOSState, i + 1, 0, LV_LABEL_ALIGN_AUTO);
        lv_table_set_cell_crop(tableOSState, i + 1, 0, true);
        lv_table_set_cell_value(tableOSState, i + 1, 0, TaskStatus_Grp[i].pcTaskName);

        uint32_t taskRunTime = TaskStatus_Grp[i].ulRunTimeCounter - TaskLastRunTime[i];
        TaskLastRunTime[i] = TaskStatus_Grp[i].ulRunTimeCounter;

        float cpu_usage = 0.0f;
        if(currentTime)
        {
            cpu_usage = (taskRunTime * 100.0f) / (currentTime);
            if(cpu_usage > 100)
                cpu_usage = 0.0f;
        }
        lv_table_set_cell_value(tableOSState, i + 1, 1, (String(cpu_usage, 1) + "%").c_str());

        const char* eCurrentStateStr[] = {"RUN", "RDY", "BLK", "SPD", "DEL", "INV"};
        lv_table_set_cell_value(tableOSState, i + 1, 2, eCurrentStateStr[TaskStatus_Grp[i].eCurrentState]);

        lv_table_set_cell_value(tableOSState, i + 1, 3, String(TaskStatus_Grp[i].uxCurrentPriority).c_str());
        lv_table_set_cell_value(tableOSState, i + 1, 4, String(TaskStatus_Grp[i].usStackHighWaterMark).c_str());
    }
}

static void Creat_TableOSState()
{
    TaskStatusNum = uxTaskGetNumberOfTasks();

    tableOSState = lv_table_create(winOSState, NULL);
    lv_table_set_style(tableOSState, LV_TABLE_STYLE_CELL1, &lv_style_plain);
    lv_table_set_style(tableOSState, LV_TABLE_STYLE_CELL2, &lv_style_plain);
    lv_table_set_style(tableOSState, LV_TABLE_STYLE_BG, &lv_style_plain_color);
    lv_table_set_col_cnt(tableOSState, TASK_INFO_COL);
    lv_table_set_row_cnt(tableOSState, TASK_INFO_ROW);
    lv_obj_set_auto_realign(tableOSState, true);
    lv_obj_align(tableOSState, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_table_set_col_width(tableOSState, 0, 80);
    lv_table_set_align(tableOSState, LV_LABEL_ALIGN_CENTER);

    const char* cell_name[TASK_INFO_COL] = {"TASK", "CPU", "STA", "PRIO", "STK"};
    for(int col = 0; col < __Sizeof(cell_name); col++)
    {
        /*Make the cells of the first row TYPE = 2 (use `style_cell2`) */
        lv_table_set_cell_type(tableOSState, 0, col, 2);
        lv_table_set_cell_value(tableOSState, 0, col, cell_name[col]);
    }

    Task_UpdateTaskStatus(0);
    /*task*/
    taskOSState = lv_task_create(Task_UpdateTaskStatus, 1000, LV_TASK_PRIO_MID, 0);
}



static bool WinOSState_AnimEnable = true;
static void WinOSState_AnimClose(bool close = true, uint16_t time = 200);

bool WinOSState_GetShow()
{
    return winOSState;
}

void WinOSState_SetClose()
{
    if(WinOSState_AnimEnable)
    {
        WinOSState_AnimClose();
    }
    else
    {
        lv_task_del(taskOSState);
        lv_obj_del_safe(&winOSState);
    }
}

static void WinOSState_AnimEvent(lv_anim_t a)
{
    WinOSState_AnimEnable = false;
    WinOSState_SetClose();
}

static void WinOSState_AnimClose(bool close, uint16_t time)
{
    lv_obj_set_opa_scale_enable(winOSState, true);

    static lv_anim_t anim_opa;
    anim_opa.var = winOSState;
    anim_opa.time = time;
    anim_opa.path_cb = lv_anim_path_ease_in_out;
    anim_opa.ready_cb = NULL;

    anim_opa.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_opa_scale;

    if(!close)
    {
        anim_opa.start = LV_OPA_TRANSP;
        anim_opa.end = LV_OPA_80;
    }
    else
    {
#define ANIM_SWAP(anim) {int temp;temp=anim.start,anim.start=anim.end,anim.end=temp;}
        ANIM_SWAP(anim_opa);
        anim_opa.ready_cb = (lv_anim_ready_cb_t)WinOSState_AnimEvent;
    }
    lv_anim_create(&anim_opa);
}

static void WinOSState_EventHandler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        if(obj == winOSState)
        {
            WinOSState_SetClose();
        }
    }
}

void Creat_WinOSState()
{
    if(WinOSState_GetShow())
        return;

    winOSState = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(winOSState, "@FreeRTOS "tskKERNEL_VERSION_NUMBER);
    lv_obj_set_size(winOSState, APP_WIN_WIDTH, APP_WIN_HEIGHT);
    lv_obj_align(winOSState, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_click(winOSState, true);
    lv_obj_set_event_cb(winOSState, WinOSState_EventHandler);

    /*如果播放音乐的时候开动画会卡*/
    if(FreeRTOS_GetCPUUsage() < 90)
    {
        WinOSState_AnimEnable = true;
        WinOSState_AnimClose(false);
    }
    else
    {
        WinOSState_AnimEnable = false;
    }

    Creat_TableOSState();
    __ExecuteOnce(AppConfigureTimerForRuntimeStats());
}


