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

int lua_destroyScript(lua_State* L)
{
    lua_getglobal(L, "__ScriptObjectPointer");
    ScriptObject* obj = static_cast<ScriptObject*>(lua_touserdata(L, -1));
    obj->destroy();
    return 0;
}

ScriptObject::ScriptObject(const char* filename)
{
    L = luaL_newstate();
    
    lua_pushlightuserdata(L, this);
    lua_setglobal(L, "__ScriptObjectPointer");
    luaL_openlibs(L);
    lua_register(L, "random", lua_random);
    lua_register(L, "destroyScript", lua_destroyScript);
    
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
        printf("ERROR(load): %s\n", luaL_checkstring(L, -1));
        destroy();
        return;
    }
    if (lua_pcall(L, 0, 0, 0))
    {
        printf("ERROR(run): %s\n", luaL_checkstring(L, -1));
        destroy();
        return;
    }
    
    lua_getglobal(L, "init");
    if (lua_pcall(L, 0, 0, 0))
    {
        printf("ERROR(init): %s\n", luaL_checkstring(L, -1));
        destroy();
        return;
    }
}

ScriptObject::~ScriptObject()
{
    lua_close(L);
}

void ScriptObject::update(float delta)
{
    lua_getglobal(L, "update");
    lua_pushnumber(L, delta);
    if (lua_pcall(L, 1, 1, 0))
    {
        printf("ERROR(update): %s\n", luaL_checkstring(L, -1));
        destroy();
        return;
    }
    lua_pop(L, 1);
}
