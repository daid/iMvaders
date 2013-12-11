#include "scriptInterfaceMagic.h"

template<> void convert<bool>::returnType(lua_State* L, bool b)
{
    lua_pushboolean(L, b);
}
