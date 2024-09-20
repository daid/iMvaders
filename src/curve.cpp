#include "curve.h"

Curve::Curve(){
    delta = 0.0;
}

glm::vec2 Curve::getPosition(float offset)
{
    float f = delta + offset;
    float g = 1.0f-f;
    return (g*g*g*p0) + (cp0*3.0f*g*g*f) + (3.0f*g*f*f*cp1) + (f*f*f*p1);
}

void Curve::moveDistance(float amount)
{
    glm::vec2 diff = getPosition() - getPosition(0.01);
    float moveDistPerPercent = sqrtf(diff.x * diff.x + diff.y * diff.y);
    delta += amount / moveDistPerPercent / 100.0f;
    if (delta < 0.0f) delta = 0.0f;
    if (delta > 1.0f) delta = 1.0f;
}

float Curve::angle()
{
    glm::vec2 diff = getPosition(0.01) - getPosition();
    return vec2ToAngle(diff) + 90;
}

void Curve::draw(sp::RenderTarget& window)
{
/*
    sf::RectangleShape debug(glm::vec2(1, 1));
    debug.setFillColor(sf::Color(255,255,255,64));
    for(unsigned int n=0; n<50; n++)
    {
        debug.setPosition(getPosition(-delta + 1.0f/50.0 * n));
        window.draw(debug);
    }
    debug.setFillColor(sf::Color(255,0,255,64));
    debug.setSize(glm::vec2(3, 3));
    debug.setPosition(cp0);
    window.draw(debug);
    debug.setPosition(cp1);
    window.draw(debug);
*/
}
