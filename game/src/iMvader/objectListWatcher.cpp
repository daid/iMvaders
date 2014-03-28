#include "scriptInterface.h"
#include "gameEntity.h"

class ListWatcher : public GameEntity
{
public:
    PVector<PObject> objects;
    float timeout;
    ScriptCallback allDestroyed;
    
    ListWatcher()
    {
        timeout = 0;
    }
    
    void add(P<PObject> obj)
    {
        objects.push_back(obj);
    }
    
    void delay(float f)
    {
        timeout = f;
    }
    
    virtual void update(float delta)
    {
        int cnt = 0;
        foreach(PObject, obj, objects)
            cnt++;
        if (cnt == 0)
        {
            if (timeout > 0)
            {
                timeout -= delta;
            }else{
                allDestroyed();
                destroy();
            }
        }
    }
};

class Delay : public GameEntity
{
public:
    float timeout;
    ScriptCallback call;
    
    Delay()
    {
        timeout = 5.0;
    }
    
    void time(float f)
    {
        timeout = f;
    }
    
    virtual void update(float delta)
    {
        if (timeout > 0)
        {
            timeout -= delta;
        }else{
            call();
            destroy();
        }
    }
};

REGISTER_SCRIPT_CLASS(ListWatcher)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(ListWatcher, add);
    REGISTER_SCRIPT_CLASS_FUNCTION(ListWatcher, delay);
    REGISTER_SCRIPT_CLASS_CALLBACK(ListWatcher, allDestroyed);
}
REGISTER_SCRIPT_CLASS(Delay)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(Delay, time);
    REGISTER_SCRIPT_CLASS_CALLBACK(Delay, call);
}
