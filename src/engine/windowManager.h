#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <SFML/Graphics.hpp>
#include "P.h"

class WindowManager : public virtual PObject
{
private:
    sf::Vector2i virtualSize;
    sf::RenderWindow window;
public:
    WindowManager(int virtualWidth, int virtualHeight, bool fullScreen);
    ~WindowManager();
    
    sf::Vector2i getVirtualSize() const { return virtualSize; }
    void render();
    void close();

    friend class InputHandler;
    friend class Engine;
};

#endif//WINDOW_MANAGER_H
