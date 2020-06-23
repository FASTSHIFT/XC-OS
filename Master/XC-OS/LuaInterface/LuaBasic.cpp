#include "Basic/TasksManage.h"
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
    ScriptText = NULL;
    ScriptFilePath = file;
    
    xTaskNotifyGive(TaskHandle_LuaScript);
}

void Task_LuaScript(void *pvParameters)
{
    for(;;)
    {
        luaScript.begin();
        LuaReg_Time();
        LuaReg_ModuleCtrl();
        LuaReg_LVGL();
        
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(ScriptText)
        {
            luaScript.doString(ScriptText);
        }
        
        if(ScriptFilePath != "")
        {
            luaScript.doFile(ScriptFilePath.c_str());
        }
        
        luaScript.end();
        lua_close(luaScript.L);
        luaScript.L = 0;
    }
}
