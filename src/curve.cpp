#include "curve.h"

Curve::Curve(){
    delta = 0.0;
}

sf::Vector2f Curve::getPosition(float offset)
{
    float f = delta + offset;
    float g = 1.0f-f;
    return (g*g*g*p0) + (cp0*3.0f*g*g*f) + (3.0f*g*f*f*cp1) + (f*f*f*p1);
}
void Curve::moveDistance(float amount)
{
    sf::Vector2f diff = getPosition() - getPosition(0.01);
    float moveDistPerPercent = sqrtf(diff.x * diff.x + diff.y * diff.y);
    delta += amount / moveDistPerPercent / 100.0f;
    if (delta < 0.0f) delta = 0.0f;
    if (delta > 1.0f) delta = 1.0f;
}

float Curve::angle()
{
    sf::Vector2f diff = getPosition(0.01) - getPosition();
    return vector2ToAngle(diff) + 90;
}

//Shouldn't this be a Renderable?
void Curve::draw(sf::RenderTarget& window)
{
    sf::RectangleShape debug(sf::Vector2f(1, 1));
    debug.setFillColor(sf::Color(255,255,255,64));
    for(unsigned int n=0; n<50; n++)
    {
        debug.setPosition(getPosition(-delta + 1.0f/50.0 * n));
        window.draw(debug);
    }
    debug.setFillColor(sf::Color(255,0,255,64));
    debug.setSize(sf::Vector2f(3, 3));
    debug.setPosition(cp0);
    window.draw(debug);
    debug.setPosition(cp1);
    window.draw(debug);
}
