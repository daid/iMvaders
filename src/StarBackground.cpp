#include "StarBackground.h"
#include "main.h"
#include "random.h"
#include "gameEntity.h"


StarBackground::StarBackground()
: Renderable(backgroundLayer)
{
    for(int n=0; n<starCount; n++)
    {
        stars[n].x = random(0, 1);
        stars[n].y = random(0, 1);
        stars[n].depth = random(0, 1);
    }
}

StarBackground::~StarBackground(){}

void StarBackground::update(float delta)
{
    for(int n=0; n<starCount; n++)
    {
        stars[n].y += (stars[n].depth + 0.4f) * delta * 0.2f;
        if (stars[n].y > 1.0f)
            stars[n].y -= 1.0f;
    }
}

void StarBackground::render(sp::RenderTarget& window)
{
    for(int n=0; n<starCount; n++)
    {
        RectangleShape rect({1, 1});
        rect.setPosition(stars[n].x * 320, stars[n].y * 240);
        rect.setFillColor({32+64*stars[n].depth,32+64*stars[n].depth,32+64*(1-stars[n].depth),255});
        rect.draw(window);
    }
}
