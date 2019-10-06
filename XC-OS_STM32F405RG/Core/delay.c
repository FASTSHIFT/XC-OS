#include "delay.h"
#include "Arduino.h"

#define SysTick_LoadValue (F_CPU / 1000U)

volatile static uint32_t System_ms = 0;

/**
  * @brief  系统滴答定时器初始化，定时1ms
  * @param  无
  * @retval 无
  */
void Delay_Init(void)
{    
    SystemCoreClockUpdate();
    SysTick_Config(SysTick_LoadValue);
    NVIC_SetPriority(SysTick_IRQn, 0);
}

/**
  * @brief  系统滴答定时器中断入口
  * @param  无
  * @retval 无
  */
void SystemMsInc(void)
{
    System_ms++;
}

/**
  * @brief  获取单片机自上电以来经过的毫秒数
  * @param  无
  * @retval 当前系统时钟毫秒数
  */
uint32_t millis(void)
{
    return System_ms;
}

/**
  * @brief  获取单片机自上电以来经过的微秒数
  * @param  无
  * @retval 当前系统时钟微秒数
  */
uint32_t micros(void)
{
    return (System_ms * 1000 + (SysTick_LoadValue - SysTick->VAL) / CYCLES_PER_MICROSECOND);
}

/**
  * @brief  毫秒级延时
  * @param  ms: 要延时的毫秒数
  * @retval 无
  */
void delay_ms(uint32_t ms)
{
    uint32_t Stop_TimePoint = System_ms + ms;
    while(System_ms < Stop_TimePoint);
}

/**
  * @brief  微秒级延时
  * @param  us: 要延时的微秒数
  * @retval 无
  */
void delay_us(uint32_t us)
{
    uint32_t total = 0;
    uint32_t target = CYCLES_PER_MICROSECOND * us;
    int last = SysTick->VAL;
    int now = last;
    int diff = 0;
start:
    now = SysTick->VAL;
    diff = last - now;
    if(diff > 0)
    {
        total += diff;
    }
    else
    {
        total += diff + SysTick_LoadValue;
    }
    if(total > target)
    {
        return;
    }
    last = now;
    goto start;
}
