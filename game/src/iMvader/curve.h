#ifndef CUBIC_BEZIER_CURVE_H
#define CUBIC_BEZIER_CURVE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "vectorUtils.h"
/**
    The Curve class holds a qubic bezier curve with a position on that curve, it is used to make enemies fly in a nice curved path.
 */
class Curve
{
public:
    sf::Vector2f p0, p1;
    sf::Vector2f cp0, cp1;
    float delta;//0.0f-1.0f

    Curve();

    sf::Vector2f getPosition(float offset=0.0f);

    //Advance the curve by a certain amount of distance, this is not 100% mathematical perfect, but it works good enough for games.
    void moveDistance(float amount);

    //Get the angle of the current curve point, in degrees.
    float angle();

    void draw(sf::RenderTarget& window);
};

#endif//CUBIC_BEZIER_CURVE_H
