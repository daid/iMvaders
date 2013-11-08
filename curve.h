#ifndef CUBIC_BEZIER_CURVE_H
#define CUBIC_BEZIER_CURVE_H
/**
    The Curve class holds a qubic bezier curve with a position on that curve, it is used to make enemies fly in a nice curved path.
 */
class Curve
{
public:
    sf::Vector2f p0, p1;
    sf::Vector2f cp0, cp1;
    float delta;//0.0f-1.0f
    
    Curve()
    {
        delta = 0.0;
    }

    sf::Vector2f getPosition(float offset=0.0f)
    {
        float f = delta + offset;
        float g = 1.0f-f;
        return (g*g*g*p0) + (cp0*3.0f*g*g*f) + (3.0f*g*f*f*cp1) + (f*f*f*p1);
    }
    
    //Advance the curve by a certain amount of distance, this is not 100% mathematical perfect, but it works good enough for games.
    void moveDistance(float amount)
    {
        sf::Vector2f diff = getPosition() - getPosition(0.01);
        float moveDistPerPercent = sqrtf(diff.x * diff.x + diff.y * diff.y);
        delta += amount / moveDistPerPercent / 100.0f;
        if (delta < 0.0f) delta = 0.0f;
        if (delta > 1.0f) delta = 1.0f;
    }
    
    //Get the angle of the current curve point, in degrees.
    float angle()
    {
        sf::Vector2f diff = getPosition() - getPosition(0.01);
        return vector2ToAngle(diff);
    }
    
    void draw(sf::RenderTarget& window)
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
};

#endif//CUBIC_BEZIER_CURVE_H
