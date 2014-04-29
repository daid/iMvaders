#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <SFML/Graphics.hpp>
#include "P.h"
#include "Renderable.h"

class WindowManager : public virtual PObject
{
private:
    sf::Vector2i virtualSize;
    sf::RenderWindow window;
    RenderChain* renderChain;
public:
    WindowManager(int virtualWidth, int virtualHeight, bool fullScreen, RenderChain* chain);
    virtual ~WindowManager();
    
    sf::Vector2i getVirtualSize() const { return virtualSize; }
    void render();
    void close();

    friend class InputHandler;
    friend class Engine;
};

#endif//WINDOW_MANAGER_H
