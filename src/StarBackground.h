#ifndef STAR_BACKGROUND_H
#define STAR_BACKGROUND_H

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
    virtual void update(float delta) override;
    virtual void render(sp::RenderTarget& window) override;
};

#endif//STAR_BACKGROUND_H
