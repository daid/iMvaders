#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include "stringImproved.h"
#include "P.h"
#include "windowManager.h"

class Engine;
extern Engine* engine;

class Engine
{
    WindowManager* windowManager;
    float q;
    
    std::map<string, P<PObject> > objectMap;
    float elapsedTime;
    float gameSpeed;
public:
    Engine();
    ~Engine();
    
    void setGameSpeed(float speed);
    float getElapsedTime();

    void registerObject(string name, P<PObject> obj);
    P<PObject> getObject(string name);
    
    void runMainLoop();
};

#endif//ENGINE_H
