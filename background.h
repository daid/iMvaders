#ifndef STAR_BACKGROUND_H
#define STAR_BACKGROUND_H

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
    
    StarBackground()
    {
        for(int n=0; n<starCount; n++)
        {
            stars[n].x = float(rand()) / float(RAND_MAX);
            stars[n].y = float(rand()) / float(RAND_MAX);
            stars[n].depth = float(rand()) / float(RAND_MAX);
        }
    }
    
    ~StarBackground()
    {
    }
    
    void render(sf::RenderWindow& window)
    {
        for(int n=0; n<starCount; n++)
        {
            sf::RectangleShape rect(sf::Vector2f(1, 1));
            rect.setPosition(stars[n].x * 320, stars[n].y * 240);
            rect.setFillColor(sf::Color(32+64*stars[n].depth,32+64*stars[n].depth,32+64*(1-stars[n].depth)));
            window.draw(rect);
            
            stars[n].y += stars[n].depth / 400 + 0.001;
            if (stars[n].y > 1.0)
                stars[n].y -= 1.0;
        }
    }
};

#endif//STAR_BACKGROUND_H
