#include "LuaScript.h"
#include "GUI/DisplayPrivate.h"
#define LUA_FUNC_MAKE(func) lua_tinker::def(luaScript.L, #func, func)

static void lua_lv_obj()
{
//    LUA_FUNC_MAKE(lv_obj_create);
//    LUA_FUNC_MAKE(lv_obj_del);
//    LUA_FUNC_MAKE(lv_obj_del_async);
//    LUA_FUNC_MAKE(lv_obj_clean);
//    LUA_FUNC_MAKE(lv_obj_invalidate);
//    LUA_FUNC_MAKE(lv_obj_set_parent);
//    LUA_FUNC_MAKE(lv_obj_move_foreground);
//    LUA_FUNC_MAKE(lv_obj_move_background);
//    LUA_FUNC_MAKE(lv_obj_set_pos);
//    LUA_FUNC_MAKE(lv_obj_set_x);
//    LUA_FUNC_MAKE(lv_obj_set_y);
//    LUA_FUNC_MAKE(lv_obj_set_size);
//    LUA_FUNC_MAKE(lv_obj_set_width);
//    LUA_FUNC_MAKE(lv_obj_set_height);
//    LUA_FUNC_MAKE(lv_obj_align);
//    LUA_FUNC_MAKE(lv_obj_align_origo);
//    LUA_FUNC_MAKE(lv_obj_realign);
//    LUA_FUNC_MAKE(lv_obj_set_auto_realign);
//    LUA_FUNC_MAKE(lv_obj_set_ext_click_area);
//    LUA_FUNC_MAKE(lv_obj_set_style);
//    LUA_FUNC_MAKE(lv_obj_refresh_style);
//    LUA_FUNC_MAKE(lv_obj_report_style_mod);
//    LUA_FUNC_MAKE(lv_obj_set_hidden);
//    LUA_FUNC_MAKE(lv_obj_set_click);
    /*...*/
}

static int lua_get_app_window()
{
    return (int)AppWindow_GetCont(PAGE_LuaAppSel);
}

static int lua_lv_label_create(int par, int obj)
{
    lv_obj_t* label = lv_label_create((lv_obj_t*)par,(lv_obj_t*)obj);
    
    lv_obj_set_auto_realign(label, true);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    return (int)label;
}

static void lua_lv_label_set_text(int label, const char * text)
{
    lv_label_set_text((lv_obj_t*)label, text);
}

static void lua_lv_label()
{
    lua_tinker::def(luaScript.L, "lv_label_create", lua_lv_label_create);
    lua_tinker::def(luaScript.L, "lv_label_set_text", lua_lv_label_set_text);
    lua_tinker::def(luaScript.L, "getAppWindow", lua_get_app_window);
//    LUA_FUNC_MAKE(lv_label_set_array_text);
//    LUA_FUNC_MAKE(lv_label_set_static_text);
//    LUA_FUNC_MAKE(lv_label_set_long_mode);
//    LUA_FUNC_MAKE(lv_label_set_align);
//    LUA_FUNC_MAKE(lv_label_set_recolor);
//    LUA_FUNC_MAKE(lv_label_set_body_draw);
//    LUA_FUNC_MAKE(lv_label_set_anim_speed);
//    LUA_FUNC_MAKE(lv_label_set_style);
    /*...*/
}

void LuaReg_LVGL()
{
    //lua_lv_obj();
    lua_lv_label();
}
