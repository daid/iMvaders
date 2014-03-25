#ifndef INPUT_H
#define INPUT_H

#include "windowManager.h"
#include "Updatable.h"

class InputHandler : public Updatable
{
    P<WindowManager> windowManager;

    sf::Vector2f mousePos;
    bool mouseButtonDown[sf::Mouse::ButtonCount];
    bool mouseButtonPressed[sf::Mouse::ButtonCount];
public:
    virtual ~InputHandler() {}
    virtual void update(float delta);
    
    sf::Vector2f getMousePos() { return mousePos; }
    bool mouseIsDown(int button) { return mouseButtonDown[button]; }
    bool mouseIsPressed(int button) { return mouseButtonPressed[button]; }
};

#endif//INPUT_H
