#include "scriptInterfaceMagic.h"

template<> void convert<bool>::returnType(lua_State* L, bool b)
{
    lua_pushboolean(L, b);
}

template<> void convert<const char*>::param(lua_State* L, int& idx, const char*& str)
{
    str = luaL_checkstring(L, idx++);
}

template<> void convert<std::string>::param(lua_State* L, int& idx, std::string& str)
{
    str = luaL_checkstring(L, idx++);
}
