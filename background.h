#ifndef STAR_BACKGROUND_H
#define STAR_BACKGROUND_H
#include <SFML/Graphics.hpp>
class Star
{
public:
    float x, y;
    float depth;
};

class StarBackground
{
public:
    const static int starCount = 256;
    Star stars[starCount];
    StarBackground();
    ~StarBackground();
    void render(sf::RenderWindow &window);
};

#endif//STAR_BACKGROUND_H
