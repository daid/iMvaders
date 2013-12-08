#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>

sf::Clock Clock;

#include "vectorUtils.h"
#include "textDraw.h"
#include "curve.h"
#include "background.h"
#include "gameEntity.h"
#include "textureManager.h"
#include "scoreManager.h"
#include "postProcessManager.h"

#include "explosion.h"
#include "bullet.h"
#include "player.h"

#include "EnemyGroup.h"
#include "BasicEnemy.h"
#include "BurstShotEnemy.h"

#include "bre.h"

#include "Updatable.h"
#include "Renderable.h"
#include "MainMenu.h"

void mainloop(sf::RenderWindow& window)
{
    new MainMenu();

    sf::Clock frameTimeClock;
    new StarBackground();
    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                window.close();
                break;
            }
        }

        float delta = frameTimeClock.getElapsedTime().asSeconds();
        frameTimeClock.restart();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
            delta /= 5.0;
        foreach(Updatable, u, updatableList)
            u->update(delta);

        // Clear the window
        window.clear(sf::Color(0, 0, 0));
        sf::RenderTarget& renderTarget = postProcessorManager.getPrimaryRenderTarget(window);
        renderTarget.clear(sf::Color(0, 0, 0));
        foreach(Renderable,r,renderableList)
            r->preRender(renderTarget);
        foreach(Renderable,r,renderableList)
            r->render(renderTarget);
        foreach(Renderable,r,renderableList)
            r->postRender(renderTarget);

#ifdef DEBUG
        foreach(GameEntity, e, entityList)
        {
            if (e->collisionRadius > 0.0)
            {
                sf::CircleShape circle(e->collisionRadius, 30);
                circle.setOrigin(e->collisionRadius, e->collisionRadius);
                circle.setPosition(e->sprite.getPosition());
                circle.setFillColor(sf::Color::Transparent);
                circle.setOutlineColor(sf::Color(255,255,255,128));
                circle.setOutlineThickness(1);
                renderTarget.draw(circle);
            }
        }
#endif
        
        postProcessorManager.postProcessRendering(window);

        // Display things on screen
        window.display();
    }
}

int main()
{
    srand(time(NULL));

    playerController[1].keyBind[0] = sf::Keyboard::A;
    playerController[1].keyBind[1] = sf::Keyboard::D;
    playerController[1].keyBind[2] = sf::Keyboard::W;
    playerController[1].keyBind[3] = sf::Keyboard::S;
    playerController[1].keyBind[4] = sf::Keyboard::Q;

    // Create the window of the application
    int gameWidth = 320*3;
    int gameHeight = 240*3;
    bool fullscreen = false;
    
    if (fullscreen)
    {
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        gameWidth = desktop.width;
        gameHeight = desktop.height;
    }

    sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "iMvaders!", sf::Style::None);
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);
    sf::View view(sf::Vector2f(160,120), sf::Vector2f(320, 240));
    if (gameWidth > gameHeight * 320 / 240)
    {
        float aspect = float(gameHeight) * 320 / 240 / float(gameWidth);
        float offset = 0.5 - 0.5 * aspect;
        view.setViewport(sf::FloatRect(offset, 0, aspect, 1));
    }else{
        float aspect = float(gameWidth) / float(gameHeight) * 240 / 320;
        float offset = 0.5 - 0.5 * aspect;
        view.setViewport(sf::FloatRect(0, offset, 1, aspect));
    }
    window.setView(view);

    mainloop(window);

    return EXIT_SUCCESS;
}
