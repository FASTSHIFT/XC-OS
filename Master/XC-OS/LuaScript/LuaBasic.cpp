#include "TasksManage.h"
#include "LuaScript.h"

TaskHandle_t TaskHandle_LuaScript = 0;

static const char *ScriptText = 0;
static String ScriptFilePath;

void LuaScriptStart(const char *script)
{
    ScriptText = script;
    ScriptFilePath = "";

    xTaskNotifyGive(TaskHandle_LuaScript);
}

void LuaScriptExecuteFile(String file)
{
    ScriptFilePath = file;
    ScriptText = NULL;
    
    xTaskNotifyGive(TaskHandle_LuaScript);
}

void Task_LuaScript(void *pvParameters)
{
    luaScript.begin();
    LuaReg_Time();
    LuaReg_GPIO();
    LuaReg_ModuleCtrl();
    LuaReg_Com();

    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(ScriptText)
        {
            luaScript.doString(ScriptText);
        }
        
        if(ScriptFilePath != "")
        {
            luaScript.doFile(ScriptFilePath.c_str());
        }
    }
}
