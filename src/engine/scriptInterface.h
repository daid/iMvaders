#ifndef SCRIPT_INTERFACE_H
#define SCRIPT_INTERFACE_H

#include "scriptInterfaceMagic.h"
#include "Updatable.h"

#define AUTO_RELOAD_SCRIPT 1

class ScriptObject : public Updatable
{
    lua_State* L;
#if AUTO_RELOAD_SCRIPT
    time_t scriptModifyTime;
#endif
public:
    ScriptObject();
    ScriptObject(const char* filename);
    virtual ~ScriptObject();
    
    void run(const char* filename);
    virtual void update(float delta);
};

class ScriptCallback
{
    P<ScriptObject> obj;
    
public:
    void call();
};

#endif//SCRIPT_INTERFACE_H
