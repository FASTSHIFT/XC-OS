#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "Module.h"

static uint16_t BrightnessTarget, BrightnessStep;

static void Task_BrightnessCtrl(lv_task_t * task)
{
    uint16_t brightness = Brightness_GetValue();
    __ValueCloseTo(brightness, BrightnessTarget, BrightnessStep);
    Brightness_SetValue(brightness);
    
    if(brightness == BrightnessTarget)
    {
        lv_task_del(task);
    }
}

void Brightness_SetGradual(uint16_t target, uint16_t time)
{
    BrightnessTarget = target;
    uint16_t length = abs((BrightnessTarget - (int)Brightness_GetValue()));
    uint16_t bps = length * 1000 / time;
    uint16_t step = bps / 100;
    BrightnessStep = step;
    lv_task_create(Task_BrightnessCtrl, 10, LV_TASK_PRIO_HIGH, 0);
}

uint16_t Brightness_GetValue()
{
    return timer_get_compare(PIN_MAP[TFT_LED_Pin].TIMx, PIN_MAP[TFT_LED_Pin].TimerChannel);
}

void Brightness_SetValue(uint16_t val)
{
    __ExecuteOnce(PWM_Init(TFT_LED_Pin, 1000, 20000));
    analogWrite(TFT_LED_Pin, val);
}
