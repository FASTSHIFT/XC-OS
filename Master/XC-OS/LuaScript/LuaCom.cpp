#include "FileGroup.h"
#include "LuaScript.h"
#include "ComPrivate.h"

//#define Lua_SERIAL Serial

//static int lua_pushstring_fmt(lua_State *L, const char *__restrict __format, ...)
//{
//    char printf_buff[100];

//    va_list args;
//    va_start(args, __format);
//    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
//    va_end(args);
//    lua_pushstring(L, printf_buff);

//    return ret_status;
//}

static int Lua_SetRFState(lua_State *L)
{
    bool rf_enable = luaL_checkinteger(L, 1);;
    CommmunicateRF_Enable(rf_enable);
    return 0;
}


static uint8_t ItemSelect_MAX = 0;
static int Lua_Handshake(lua_State *L)
{
    CommmunicateRF_Enable(false);
    ItemSelect_MAX = 0;

    /*主机准备握手*/
    HandshakeRun(HandshakeState_Prepare);

    uint32_t StopTime = millis() + luaL_checkinteger(L, 1);
    while(millis() < StopTime)
    {
        /*获取从机列表数量*/
        ItemSelect_MAX = HandshakeRun(HandshakeState_Search);
    }
    lua_pushnumber(L, ItemSelect_MAX);
    return 1;
}

static int Lua_ConnectSlave(lua_State *L)
{
    if(ItemSelect_MAX == 0)
    {
        lua_pushstring(L, "No slave has found! please retry handshake");
        lua_error(L);
    }

    uint8_t ItemSelect = luaL_checkinteger(L, 1);
    if(ItemSelect >= ItemSelect_MAX)
    {
        lua_pushfstring(L, "slave num max is %d!\r\n", ItemSelect_MAX - 1);
        lua_error(L);
    }

    /*超时设置*/
    uint32_t timeout = millis();
    bool IsTimeout = false;
    /*等待从机响应握手信号*/
    while(HandshakeRun(HandshakeState_ReqConnect, ItemSelect, CMD_AttachConnect) != CMD_AgreeConnect)
    {
        /*2500ms超时*/
        if(millis() - timeout > 2500)
        {
            IsTimeout = true;
            break;
        }
    }

    /*握手收尾设置，跳转至约定好的握手频率以及地址*/
    HandshakeRun(HandshakeState_Connected);

    /*对应从机类型*/
    CTRL.Info.CtrlObject = HandshakePack_Slave[ItemSelect].CtrlType;
    
    lua_pushboolean(L, !IsTimeout);
    return 1;
}

static int Lua_GetSlaveInfo(lua_State *L)
{
    uint8_t ItemSelect = lua_tonumber(L, 1);
    if(ItemSelect >= ItemSelect_MAX)
    {
        lua_pushstring(L, "Error slave require");
        lua_error(L);
    }
    lua_pushinteger(L, HandshakePack_Slave[ItemSelect].ID);
    lua_pushstring(L, HandshakePack_Slave[ItemSelect].Description);
    lua_pushinteger(L, HandshakePack_Slave[ItemSelect].CtrlType);
    return 3;
}

static int Lua_SetChannle(lua_State *L)
{
//    SetJoystickConnectEnable(false);
    uint8_t channle = luaL_checkinteger(L, 1);
    int16_t value = luaL_checkinteger(L, 2);

    switch(channle)
    {
    case 0:
        CTRL.Left.X = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    case 1:
        CTRL.Left.Y = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    case 2:
        CTRL.Right.X = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    case 3:
        CTRL.Right.Y = constrain(value, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        break;
    default:
        lua_pushstring(L, "Error Channle num");
        lua_error(L);
        break;
    }

    return 0;
}

static int Lua_GetChannle(lua_State *L)
{
    uint8_t channle = luaL_checkinteger(L, 1);
    int16_t value = 0;

    switch(channle)
    {
//    case 0:
//        value = JS_L.X;
//        break;
//    case 1:
//        value = JS_L.Y;
//        break;
//    case 2:
//        value = JS_R.X;
//        break;
//    case 3:
//        value = JS_R.Y;
//        break;
    default:
        lua_pushstring(L, "Error channle num");
        lua_error(L);
        break;
    }

    lua_pushinteger(L, value);

    return 1;
}

static int Lua_SetButton(lua_State *L)
{
    uint8_t bt = constrain(luaL_checkinteger(L, 1), 0, 7);
    bool val = luaL_checkinteger(L, 2);

    if(val)
    {
        CTRL.Key |= 1 << bt;
    }
    else
    {
        CTRL.Key &= ~(1 << bt);
    }

    return 0;
}

void LuaReg_Com()
{
    luaScript.registerFunc("setRF", Lua_SetRFState);
    luaScript.registerFunc("handshake", Lua_Handshake);
    luaScript.registerFunc("connect", Lua_ConnectSlave);
    luaScript.registerFunc("getSlave", Lua_GetSlaveInfo);
    luaScript.registerFunc("setCh", Lua_SetChannle);
    luaScript.registerFunc("setBt", Lua_SetButton);
    luaScript.registerFunc("getCh", Lua_GetChannle);
}
