#include "Basic/FileGroup.h"
#include "LuaScript.h"
#include "Module/Module.h"

static int Lua_MotorVibrate(lua_State *L)
{
    Motor_Vibrate(luaL_checknumber(L, 1), luaL_checkinteger(L, 2));
    return 0;
}

void LuaReg_ModuleCtrl()
{
    luaScript.registerFunc("motor", Lua_MotorVibrate);
}
