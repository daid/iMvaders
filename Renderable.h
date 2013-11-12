#ifndef RENDERABLE_H
#define RENDERABLE_H
#include <SFML/Graphics.hpp>
#include "P.h"

//Abstract class for entity that can be rendered.
class Renderable: public virtual PObject
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
