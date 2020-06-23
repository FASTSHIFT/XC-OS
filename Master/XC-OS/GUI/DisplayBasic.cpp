#include "DisplayPrivate.h"
#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "BSP/BSP.h"

TaskHandle_t TaskHandle_Display = 0;
TaskHandle_t TaskHandle_PageRun = 0;
QueueHandle_t SemHandle_FileSystem = 0;

SCREEN_CLASS screen(
    TFT_PORT_Pin, TFT_RST_Pin, TFT_CS_Pin,
    TFT_RS_Pin, TFT_RW_Pin, TFT_RD_Pin
);

PageManager page(PAGE_MAX);

#define PAGE_REG(name)\
do{\
    extern void PageRegister_##name(uint8_t pageID);\
    PageRegister_##name(PAGE_##name);\
}while(0)

static void Init_Pages()
{
    PAGE_REG(Home);
    PAGE_REG(Settings);
    PAGE_REG(BattInfo);
    PAGE_REG(LuaScript);
    //PAGE_REG(LuaAppSel);
    //PAGE_REG(LuaAppWin);
    PAGE_REG(FileExplorer);
    PAGE_REG(TextEditor);
    PAGE_REG(WavPlayer);
    PAGE_REG(BvPlayer);
    PAGE_REG(Game);
    PAGE_REG(GameSelect);
    PAGE_REG(About);
    PAGE_REG(RadioCfg);
    PAGE_REG(Shell);
    PAGE_REG(SubDev);
    
    page.PagePush(PAGE_Home);
}

void Page_Delay(uint32_t ms)
{
    vTaskDelay(ms);
}

void Task_Display(void *pvParameters)
{
    DisplayError_Init();
    
    screen.begin();
    screen.fillScreen(screen.Black);
    tp_dev.init();
    
    Backlight_SetValue(0);

    lv_init();
    lv_disp_init();
    lv_theme_set_current(lv_theme_material_init(200, NULL));
    lv_fsys_init();
    SemHandle_FileSystem = xSemaphoreCreateBinary();
    xSemaphoreGive(SemHandle_FileSystem);

    Init_Bar();
    Init_Pages();
    
    Backlight_SetGradual(500);

    for(;;)
    {
        lv_task_handler();
        vTaskDelay(10);
    }
}

void Task_PageRun(void *pvParameters)
{
    for(;;)
    {
        page.Running();
        vTaskDelay(20);
    }
}
