#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "LuaScript.h"
#include "BSP.h"

/*�����Ϣ��������*/
static void Task_BattUpdate(lv_task_t * task);
static lv_task_t * taskBattUpdate;

/*��������ѹ��ֵ��ʾ*/
static lv_obj_t * labelCurrent;
static lv_obj_t * labelVoltage;

/*��ŵ�״̬��ʾ*/
static lv_obj_t * labelStatus;
static lv_obj_t * ledStatus;

/*������ʾ*/
static lv_obj_t * arcBattUsage;
static lv_obj_t * labelBattUsage;

static void Creat_Label(
    lv_obj_t** label,
    lv_obj_t * alignObj,
    lv_align_t align,
    lv_coord_t x,
    lv_coord_t y
)
{
    *label = lv_label_create(appWindow, NULL);
    lv_label_set_text(*label, "");
    lv_obj_align(*label, alignObj, align, x, y);
    lv_obj_set_auto_realign(*label, true);
}

void Creat_ArcBatt(lv_obj_t** arc, lv_obj_t * parent, int16_t val)
{
    /*Create style for the Arcs*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.color = LV_COLOR_NAVY;           /*Arc color*/
    style.line.width = 8;                       /*Arc width*/

    /*Create an Arc*/
    *arc = lv_arc_create(parent, NULL);
    lv_arc_set_style(*arc, LV_ARC_STYLE_MAIN, &style);
    lv_obj_align(*arc, NULL, LV_ALIGN_CENTER, 0, 0);

    if(val >= 359) val = 359;
    if(val < 180) lv_arc_set_angles(*arc, 180 - val, 180);
    else lv_arc_set_angles(*arc, 540 - val, 180);
}

static int batCur, batVol;

static void Task_BattUpdate(lv_task_t * task)
{
    batCur = __Map(ABS(BattCurret), 0, 3000, 0, 100);
    batVol = __Map(BattVoltage, 2600, 4200, 0, 100);


    lv_label_set_text_fmt(labelCurrent, "%0.2fmA", BattCurret);
    lv_label_set_text_fmt(labelVoltage, "%0.2fmV", BattVoltage);

    float power = BattCurret * BattVoltage / 1000000.0f;
    const char *state = BattCurret < 0.0f ? "Discharge" : "Charge";
    lv_label_set_text_fmt(labelStatus, "%s: %0.2fW", state,power);
}

/**
  * @brief  ҳ���ʼ���¼�
  * @param  ��
  * @retval ��
  */
static void Setup()
{
    Creat_ArcBatt(&arcBattUsage, appWindow, map(BattVoltageOc, 2600, 4200, 0, 360));
    Creat_Label(&labelStatus, arcBattUsage, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    taskBattUpdate = lv_task_create(Task_BattUpdate, 500, LV_TASK_PRIO_MID, 0);
}

/**
  * @brief  ҳ���˳��¼�
  * @param  ��
  * @retval ��
  */
static void Exit()
{
    lv_obj_del_safe(&labelStatus);
    lv_task_del(taskBattUpdate);
}

/**
  * @brief  ҳ���¼�
  * @param  event:�¼����
  * @param  param:�¼�����
  * @retval ��
  */
static void Event(int event, void* param)
{
    lv_obj_t * btn = (lv_obj_t*)param;
    if(event == LV_EVENT_CLICKED)
    {
        if(btn == btnBack)
        {
            page.PagePop();
        }
    }
}

/**
  * @brief  ҳ��ע��
  * @param  pageID:Ϊ��ҳ������ID��
  * @retval ��
  */
void PageRegister_BattInfo(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
