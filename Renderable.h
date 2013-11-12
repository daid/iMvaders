#ifndef RENDERABLE_H
#define RENDERABLE_H
#include <SFML/Graphics.hpp>

//Abstract class for entity that can be rendered.
class Renderable
{
    public:
        Renderable();
        virtual ~Renderable();
        virtual void preRender(sf::RenderTarget& window) = 0;
        virtual void render(sf::RenderTarget& window) = 0;
        virtual void postRender(sf::RenderTarget& window) = 0;
    protected:
    private:
};

#endif // RENDERABLE_H
