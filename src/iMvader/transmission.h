#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "Updatable.h"
#include "Renderable.h"

class Transmission : public Updatable, public Renderable
{
    sf::Sprite face;
    std::string faceName;
    std::string text;
    float transmissionTime;
    
    const static float charsPerSecond = 10.0f;
public:
    Transmission();
    virtual ~Transmission() {}
    
    virtual void preRender(sf::RenderTarget& window) {}
    virtual void render(sf::RenderTarget& window) {}
    virtual void postRender(sf::RenderTarget& window);
    
    virtual void update(float delta);
};

#endif//TRANSMISSION_H
