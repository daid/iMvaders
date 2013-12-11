#ifndef SCRIPT_INTERFACE_H
#define SCRIPT_INTERFACE_H

#include "scriptInterfaceMagic.h"
#include "Updatable.h"

class ScriptObject : public Updatable
{
    lua_State* L;
public:
    ScriptObject(const char* filename);
    virtual ~ScriptObject();
    
    virtual void update(float delta);
};

#endif//SCRIPT_INTERFACE_H
