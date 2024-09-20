#include "textDraw.h"
#include "textureManager.h"
#include "gameEntity.h"

static int textWidth(string str)
{
    int width = 0;
    int ret = 0;
    for(unsigned int i=0; i<str.size(); i++)
    {
        int c = str[i];
        if (c >= 'A' && c <= 'Z')
            ret += spriteManager.getSpriteSize("abc", c-'A').x + 1;
        else if (c >= 'a' && c <= 'z')
            ret += spriteManager.getSpriteSize("abc", c-'a').x + 1;
        else if (c >= '0' && c <= '9')
            ret += spriteManager.getSpriteSize("nums", c-'0').x + 1;
        else if (c == ':')
            ret += 5;
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

void drawText(sp::RenderTarget& window, float x, float y, string str, textAlign align, float scale)
{
    Sprite letter;
    if (align == align_center)
        x -= textWidth(str)/2.0f * scale;
    if (align == align_right)
        x -= textWidth(str) * scale;
    float xStart = x;
    for(unsigned int i=0; i<str.size(); i++)
    {
        int c = str[i];
        if (c >= 'A' && c <= 'Z')
        {
            c -= 'A';
            spriteManager.setTexture(letter, "abc", c);
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            letter.setScale(scale, scale);
            x += (letter.size.x + 1.0f) * scale;
            letter.draw(window);
        }
        else if (c >= 'a' && c <= 'z')
        {
            c -= 'a';
            spriteManager.setTexture(letter, "abc", c);
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            letter.setScale(scale, scale);
            x += (letter.size.x + 1.0f) * scale;
            letter.draw(window);
        }
        else if (c >= '0' && c <= '9')
        {
            spriteManager.setTexture(letter, "nums", c-'0');
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            letter.setScale(scale, scale);
            x += (letter.size.x + 1.0f) * scale;
            letter.draw(window);
        }
        else if (c == '|')
        {
            x = xStart;
            y += 15.0f * scale;
        }
        else if (c == ':')
            x += 5.0f * scale;
        else
            x += 8.0f * scale;
    }
}
