#include "textDraw.h"
#include "textureManager.h"

static int textWidth(sf::String str)
{
    int width = 0;
    int ret = 0;
    for(unsigned int i=0; i<str.getSize(); i++)
    {
        int c = str[i];
        if (c >= 'A' && c <= 'Z')
            ret += textureManager.getSpriteRect("abc", c-'A').width + 1;
        else if (c >= 'a' && c <= 'z')
            ret += textureManager.getSpriteRect("abc", c-'A').width + 1;
        else if (c >= '0' && c <= '9')
            ret += textureManager.getSpriteRect("nums", c-'0').width + 1;
        else if (c == '|')
        {
            width = std::max(ret, width);
            ret = 0;
        }
        else
            ret += 8;
    }
    return std::max(ret, width);
}

void drawText(sf::RenderTarget& window, float x, float y, sf::String str, textAlign align)
{
    sf::Sprite letter;
    if (align == align_center)
        x -= textWidth(str)/2.0f;
    if (align == align_right)
        x -= textWidth(str);
    float xStart = x;
    for(unsigned int i=0; i<str.getSize(); i++)
    {
        int c = str[i];
        if (c >= 'A' && c <= 'Z')
        {
            c -= 'A';
            textureManager.setTexture(letter, "abc", c);
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            x += letter.getTextureRect().width + 1;
            window.draw(letter);
        }
        else if (c >= 'a' && c <= 'z')
        {
            c -= 'a';
            textureManager.setTexture(letter, "abc", c);
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            x += letter.getTextureRect().width + 1;
            window.draw(letter);
        }
        else if (c >= '0' && c <= '9')
        {
            textureManager.setTexture(letter, "nums", c-'0');
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            x += letter.getTextureRect().width + 1;
            window.draw(letter);
        }
        else if (c == '|')
        {
            x = xStart;
            y += 15;
        }
        else
        {
            x += 8;
        }
    }
}
