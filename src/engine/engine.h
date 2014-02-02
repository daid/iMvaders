#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include "P.h"
#include "windowManager.h"

class Engine;
extern Engine* engine;

class Engine
{
    WindowManager* windowManager;
    
    std::map<std::string, P<PObject> > objectMap;
    float gameSpeed;
public:
    Engine();
    ~Engine();
    
    void setGameSpeed(float speed);

    void registerObject(std::string name, P<PObject> obj);
    P<PObject> getObject(std::string name);
    
    void runMainLoop();
};

#endif//ENGINE_H
