#include "Basic/FileGroup.h"
#include "LuaScript.h"

static int GetPin(const char* str)
{
    if(strlen(str) != 2)
        return -1;
    
    if(
        str[0] < 'A' || str[0] > 'Z' 
     || str[1] < '0' || str[1] > '9')
    {
        return -1;
    }
    
    int GPIOx = str[0] - 'A';
    int Pinx = str[1] - '0';

    return (GPIOx * 16 + Pinx);
}

static int GetPinMode(const char* str)
{
    typedef struct{
        const char* str;
        pinMode_TypeDef mode;
    }ModeMap_t;
    
    const ModeMap_t map[]={
        {"INPUT", INPUT},
        {"INPUT_PULLUP", INPUT_PULLUP},
        {"INPUT_PULLDOWN", INPUT_PULLDOWN},
        {"INPUT_ANALOG", INPUT_ANALOG},
        {"INPUT_ANALOG_DMA", INPUT_ANALOG_DMA},
        {"OUTPUT", OUTPUT},
        {"OUTPUT_OPEN_DRAIN", OUTPUT_OPEN_DRAIN},
        {"OUTPUT_AF", OUTPUT_AF},
        {"PWM", PWM},
    };
    
    for(int i; i < __Sizeof(map);i++)
    {
        if(strcmp(str, map[i].str)==0)
        {
            return map[i].mode;
        }
    }
    return 0;
}

static void Lua_togglePin(uint8_t pin)
{
    if(IS_PIN(pin))
        togglePin(pin);
}

void LuaReg_GPIO()
{
    lua_tinker::def(luaScript.L, "P", GetPin);
    lua_tinker::def(luaScript.L, "Mode", GetPinMode);
    lua_tinker::def(luaScript.L, "pinMode", pinMode);
    lua_tinker::def(luaScript.L, "togglePin", Lua_togglePin);
    lua_tinker::def(luaScript.L, "digitalWrite", digitalWrite);
    lua_tinker::def(luaScript.L, "digitalRead", digitalRead);
    lua_tinker::def(luaScript.L, "analogWrite", analogWrite);
    lua_tinker::def(luaScript.L, "analogRead", analogRead);
//    luaScript.registerFunc("pinMode", Lua_pinMode);
//    luaScript.registerFunc("togglePin", Lua_togglePin);
//    luaScript.registerFunc("digitalWrite", Lua_digitalWrite);
//    luaScript.registerFunc("digitalRead", Lua_digitalRead);
//    luaScript.registerFunc("analogWrite", Lua_analogWrite);
//    luaScript.registerFunc("analogRead", Lua_analogRead);
}
