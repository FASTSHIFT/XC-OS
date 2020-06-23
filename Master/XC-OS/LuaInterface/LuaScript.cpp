#include "LuaScript.h"
#include "Arduino.h"

void lua_string_print(const char *s)
{
    //Serial.print(s);
    luaScript.printStr(s);
}

LuaScript::LuaScript()
{
    L = NULL;
    isRunning = false;
    isBegun = false;
    stringPrint = 0;
}

LuaScript::~LuaScript()
{
    end();
    lua_close(L);
    L = NULL;
}

bool LuaScript::begin()
{
//    if(isBegun)
//        return true;

    L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_base(L);
    
    isBegun = true;
    
    return false;
}

static void lua_break_callback(lua_State *L, lua_Debug *ar)
{
    lua_sethook(L, NULL, 0, 0);
    if(luaScript.isRunning)
        luaL_error(L, "lua script break");
}

bool LuaScript::end()
{
    if(!isRunning)
        return false;
    
    int mask = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE | LUA_MASKCOUNT;
    lua_sethook(L, lua_break_callback, mask, 1);
    return true;
}

bool LuaScript::doFile(const char *file)
{
    isRunning = true;
    bool error = luaL_dofile(L, file);
    isRunning = false;
    if(error)
        printStr(lua_tostring(L, -1));

    return error;
}

bool LuaScript::doString(const char *s)
{
    isRunning = true;
    bool error = luaL_dostring(L, s);
    isRunning = false;
    if(error)
        printStr(lua_tostring(L, -1));

    return error;
}

void LuaScript::registerStrOutput(StrOutput_Callback_t func)
{
    stringPrint = func;
}

void LuaScript::registerFunc(const char *name, lua_CFunction func)
{
    lua_register(L, name, func);
}

void LuaScript::printStr(const char *s)
{
    if(stringPrint)
        stringPrint(s);
}

LuaScript luaScript;
