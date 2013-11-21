#include "background.h"
#include "random.h"

StarBackground::StarBackground()
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
        stars[n].y += (stars[n].depth + 0.4) * delta * 0.2;
        if (stars[n].y > 1.0)
            stars[n].y -= 1.0;
    }
}

void StarBackground::preRender(sf::RenderTarget& window)
{
    for(int n=0; n<starCount; n++)
    {
        sf::RectangleShape rect(sf::Vector2f(1, 1));
        rect.setPosition(stars[n].x * 320, stars[n].y * 240);
        rect.setFillColor(sf::Color(32+64*stars[n].depth,32+64*stars[n].depth,32+64*(1-stars[n].depth)));
        window.draw(rect);
    }
}
