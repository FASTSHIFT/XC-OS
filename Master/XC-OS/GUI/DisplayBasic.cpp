#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "TasksManage.h"
#include "Module.h"

TaskHandle_t TaskHandle_Display = 0;
TaskHandle_t TaskHandle_PageRun = 0;
QueueHandle_t SemHandle_FileSystem = 0;

SCREEN_CLASS screen(
    TFT_PORT_Pin, TFT_RST_Pin, TFT_CS_Pin,
    TFT_RS_Pin, TFT_RW_Pin, TFT_RD_Pin
);

PageManager page(PAGE_MAX);

static void Init_Pages()
{
    PAGE_REG(Home);
    PAGE_REG(Settings);
    PAGE_REG(BattInfo);
    PAGE_REG(LuaScript);
    PAGE_REG(SetDisplay);
    PAGE_REG(FileExplorer);
    PAGE_REG(SubAPPs);
    PAGE_REG(TextEditor);
    PAGE_REG(WavPlayer);
    PAGE_REG(BvPlayer);
    PAGE_REG(Game);
    PAGE_REG(About);
    
    page.PagePush(PAGE_Home);
}

void Task_PageRun(void *pvParameters)
{
    for(;;)
    {
        page.Running();
        vTaskDelay(20);
    }
}



void Task_Dispaly(void *pvParameters)
{
    screen.begin();
    screen.fillScreen(screen.Black);
    tp_dev.init();
    
    Brightness_SetValue(0);

    lv_user_init();
    
    lv_user_fs_init();
    SemHandle_FileSystem = xSemaphoreCreateBinary();
    xSemaphoreGive(SemHandle_FileSystem);

    lv_theme_set_current(lv_theme_material_init(200, LV_FONT_DEFAULT));
    
    Init_Bar();
    Init_Pages();
    
    Brightness_SetGradual(500);

    for(;;)
    {
        lv_task_handler();
        vTaskDelay(10);
    }
}
