#include "random.h"
#include "scriptInterface.h"

registerObjectFunctionListItem* registerObjectFunctionListStart;

int lua_random(lua_State* L)
{
    float rMin = luaL_checknumber(L, 1);
    float rMax = luaL_checknumber(L, 2);
    lua_pushnumber(L, random(rMin, rMax));
    return 1;
}

void runScript(const char* filename)
{
    lua_State* L = luaL_newstate();
    
    luaL_openlibs(L);
    lua_register(L, "random", lua_random);
    
    int maxPrio = 0;
    for(int prio=0; prio<=maxPrio; prio++)
    {
        for(registerObjectFunctionListItem* item = registerObjectFunctionListStart; item != NULL; item = item->next)
        {
            if (item->prio == prio)
                item->func(L);
            if (item->prio > maxPrio)
                maxPrio = item->prio;
        }
    }
    
    if (luaL_loadfile(L, filename))
    {
        printf("ERROR: %s\n", luaL_checkstring(L, -1));
        lua_close(L);
        return;
    }
    if (lua_pcall(L, 0, 0, 0))
    {
        printf("ERROR: %s\n", luaL_checkstring(L, -1));
        lua_close(L);
        return;
    }
    lua_close(L);
}

template<> void convert<sf::Vector2f>::param(lua_State* L, int& idx, sf::Vector2f& v)
{
    v.x = luaL_checknumber(L, idx++);
    v.y = luaL_checknumber(L, idx++);
}
