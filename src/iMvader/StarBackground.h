#ifndef STAR_BACKGROUND_H
#define STAR_BACKGROUND_H

#include <SFML/Graphics.hpp>
#include "Updatable.h"
#include "Renderable.h"

class Star
{
public:
    float x, y;
    float depth;
};

class StarBackground : public Updatable, Renderable
{
public:
    const static int starCount = 256;
    Star stars[starCount];
    StarBackground();
    virtual ~StarBackground();
    virtual void update(float delta);
    virtual void render(sf::RenderTarget& window);
};

#endif//STAR_BACKGROUND_H
