#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <SFML/Graphics.hpp>

class WindowManager : public sf::NonCopyable
{
private:
    sf::Vector2i virtualSize;
    sf::RenderWindow window;
public:
    WindowManager(int virtualWidth, int virtualHeight, bool fullScreen);
    ~WindowManager();
    
    void mainLoop();

    friend class InputHandler;
};

#endif//WINDOW_MANAGER_H
