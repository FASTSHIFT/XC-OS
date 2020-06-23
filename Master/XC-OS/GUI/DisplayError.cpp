#include "DisplayPrivate.h"
#include "Basic/TasksManage.h"
#include "cm_backtrace.h"

void DisplayError_Init()
{
    cm_backtrace_init(XC_OS_NAME, XC_VERSION_HW, XC_VERSION_SW" "__DATE__);
}

void cmb_printf_hook(const char *__restrict __format, ...)
{
    char printf_buff[256];

    va_list args;
    va_start(args, __format);
    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
    va_end(args);
    Serial.print(printf_buff);
}

#define BlueScreeDelayTime 10000

static void SoftDelay(uint32_t ms)
{
    volatile uint32_t i = F_CPU / 1000 * ms / 5;
    while(i--);
}

#include "Fonts\FreeMono24pt7b.h"
static void DisplayCrashReports(const char* report)
{
    noInterrupts();
    screen.fillScreen(screen.Blue);
    screen.setTextColor(screen.White);
    screen.setFont(&FreeMono24pt7b);
    screen.setTextSize(2);
    screen.setCursor(0, 80);
    screen.println(":(");

    screen.setFont();
    screen.setTextSize(2);

    screen.setCursor(0, ScreenMid_H - TEXT_HEIGHT_1 * 2);
    screen.println(report);
    screen.print("Syetem will reboot...");

    screen.setCursor(0, screen.height() - TEXT_HEIGHT_1 * 2 * 6 - 2);
    screen.println("Error code:");
    screen.printf("MMFAR=0x%x\r\n", SCB->MMFAR);
    screen.printf("BFAR=0x%x\r\n", SCB->BFAR);
    screen.printf("CFSR=0x%x\r\n", SCB->CFSR);
    screen.printf("HFSR=0x%x\r\n", SCB->HFSR);
    screen.printf("DFSR=0x%x\r\n", SCB->DFSR);
    
    SoftDelay(BlueScreeDelayTime);
    NVIC_SystemReset();
}


/***************************** Hooks *******************************/
extern "C"
{
    void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
    {
        char str[configMAX_TASK_NAME_LEN + 1];
        sprintf(str, "stack overflow\n < %s >", pcTaskName);
        DisplayCrashReports(str);
    }
    
    void vApplicationMallocFailedHook()
    {
        DisplayCrashReports("malloc failed");
    }
    
    void vApplicationHardFaultHook()
    {
        DisplayCrashReports("FXXK HardFault!");
    }
    
    __asm void HardFault_Handler()
    {
        extern vApplicationHardFaultHook
        extern cm_backtrace_fault
            
        mov r0, lr
        mov r1, sp
        bl cm_backtrace_fault
        bl vApplicationHardFaultHook
Fault_Loop
        b Fault_Loop
    }
}
