#ifndef TEXT_DRAW_H
#define TEXT_DRAW_H

#include <stdio.h>
#include <SFML/Graphics.hpp>

enum textAlign
{
    align_center = 0,
    align_left = 1,
    align_right = 2
};

void drawText(sf::RenderTarget& window, float x, float y, sf::String str, textAlign align = align_center);

/* Utility function in assisting to convert numbers to strings */
static inline sf::String to_string(int number)
{
    char buf[16];
    itoa(number, buf, 10);
    return sf::String(buf);
}

static inline sf::String to_string(double number)
{
    char buf[16];
    sprintf(buf, "%.2f", number);
    return sf::String(buf);
}

#endif//TEXT_DRAW_H
