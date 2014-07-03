#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "gameEntity.h"
#include "scriptInterface.h"

class Transmission : public GameEntity
{
    sf::Sprite face;
    string faceName;
    string text;
    float transmissionTime;
    float positionY;

    const static float charsPerSecond = 10.0f;
    const static float extraDelayBeforeDone = 2.0f;
public:
    ScriptCallback transmissionDone;

    Transmission();
    virtual ~Transmission() {}
    
    void setFace(const char* faceName);
    void setText(const char* text);
    void top();
    void bottom();
    
    virtual void render(sf::RenderTarget& window);
    
    virtual void update(float delta);
};

#endif//TRANSMISSION_H
