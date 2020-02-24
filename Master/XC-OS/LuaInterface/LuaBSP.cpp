#include "Basic/FileGroup.h"
#include "LuaScript.h"
#include "BSP/BSP.h"

void LuaReg_ModuleCtrl()
{
    lua_tinker::def(luaScript.L, "motor", Motor_Vibrate);
}
