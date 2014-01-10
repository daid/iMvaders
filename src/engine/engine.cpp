#include "engine.h"
#include "Updatable.h"
#include "Collisionable.h"

Engine* engine;

Engine::Engine()
{
    engine = this;
    windowManager = NULL;
}
Engine::~Engine()
{
    delete windowManager;
}

void Engine::registerObject(std::string name, P<PObject> obj)
{
    objectMap[name] = obj;
}

P<PObject> Engine::getObject(std::string name)
{
    if (!objectMap[name])
        return NULL;
    return objectMap[name];
}

void Engine::runMainLoop()
{
    windowManager = dynamic_cast<WindowManager*>(*getObject("windowManager"));
    sf::Clock frameTimeClock;
    while (windowManager->window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (windowManager->window.pollEvent(event))
        {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                windowManager->window.close();
                break;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            windowManager->window.close();

        float delta = frameTimeClock.getElapsedTime().asSeconds();
        frameTimeClock.restart();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
            delta /= 5.0;
        foreach(Updatable, u, updatableList)
            u->update(delta);
        Collisionable::handleCollisions();

        // Clear the window
        windowManager->render();
    }
}
