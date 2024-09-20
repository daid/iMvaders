#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "gameEntity.h"
#include "scriptInterface.h"

class Transmission : public GameEntity
{
    Sprite face;
    string faceName;
    string text;
    float transmissionTime;
    float positionY;

    constexpr static float charsPerSecond = 10.0f;
    constexpr static float extraDelayBeforeDone = 2.0f;
public:
    ScriptCallback transmissionDone;

    Transmission();
    virtual ~Transmission() {}
    
    void setFace(const char* faceName);
    void setText(const char* text);
    void top();
    void bottom();
    
    virtual void render(sp::RenderTarget& window) override;
    
    virtual void update(float delta) override;
};

#endif//TRANSMISSION_H
