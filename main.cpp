#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>

//#define DEBUG

float random(float fmin, float fmax)
{
    return (float(rand()) / float(RAND_MAX)) * (fmax - fmin) + fmin;
}

#include "vectorUtils.h"
#include "curve.h"
#include "background.h"
#include "gameEntity.h"

sf::Texture logoTexture;
sf::Texture bulletTexture;
sf::Texture bre1Texture;
sf::Texture bre2Texture;
sf::Texture insertCoinTexture;
sf::Texture invaderTexture;
sf::Texture invaderShieldedTexture;
sf::Texture playerTexture;
sf::Clock Clock;

#include "bullet.h"
#include "player.h"
#include "enemy.h"
#include "bre.h"

PlayerController playerController[2];

class mainMenu : public GameEntity
{
public:
    sf::Sprite logoSprite;
    sf::Sprite insertCoinSprite;
    PVector<Enemy> enemyList;
    P<Enemy> lastSpawn;
    sf::Vector2f enemyStartPosition;
    bool startGame;
    int enemyDelay, enemyWaveState, enemySpawnCount;
    
    mainMenu()
    : enemyStartPosition(50, -20), enemyDelay(0), enemyWaveState(0), enemySpawnCount(10)
    {
        startGame = false;
        
        logoSprite.setTexture(logoTexture);
        logoSprite.setOrigin(logoTexture.getSize().x / 2, 0);
        logoSprite.setPosition(160, 40);
        
        insertCoinSprite.setTexture(insertCoinTexture);
        insertCoinSprite.setOrigin(insertCoinTexture.getSize().x / 2, 0);
        insertCoinSprite.setPosition(160, 200);
    }
    
    ~mainMenu()
    {
    }
    
    virtual void update()
    {
        updateEnemies();
        
        if (!startGame)
        {
            if (playerController[0].fire())
                startGame = true;
        }
        if (startGame && enemyList.size() == 0)
        {
            foreach(GameEntity, e, entityList)
                e->destroy();
            new PlayerCraft(&playerController[0]);
        }
    }
    
    void updateEnemies()
    {
        if (enemyDelay && !startGame)
        {
            enemyDelay--;
            return;
        }
        //If we are starting, call the update for each enemy another time, so the fly double as fast.
        if (startGame)
            foreach(Enemy, e, enemyList)
                e->update();
        
        switch(enemyWaveState)
        {
        case 0:
            if (lastSpawn)
            {
                if (lastSpawn->flightCurve.delta == 1.0f)
                    lastSpawn = NULL;
                else if ((lastSpawn->sprite.getPosition() - enemyStartPosition) > 20.0f)
                    lastSpawn = NULL;
            }
            if (!lastSpawn)
            {
                if (enemySpawnCount > 0 && !startGame)
                {
                    lastSpawn = new Enemy();
                    lastSpawn->flightCurve.p0 = enemyStartPosition;
                    lastSpawn->flightCurve.cp0 = enemyStartPosition + sf::Vector2f(0, 140);
                    lastSpawn->flightCurve.cp1 = sf::Vector2f(160-5*30 + enemySpawnCount * 30, 200);
                    lastSpawn->flightCurve.p1 = sf::Vector2f(160-5*20 + enemySpawnCount * 20, 100);
                    enemyList.push_back(lastSpawn);
                    enemySpawnCount--;
                }
                else
                {
                    enemyDelay = 300;
                    enemyWaveState = 1;
                }
            }
            break;
        case 1:
            foreach(Enemy, e, enemyList)
                if (e->flightCurve.delta != 1.0f)
                    return;

            foreach(Enemy, e, enemyList)
            {
                e->flightCurve.delta = 0.0f;
                e->flightCurve.p0 = e->flightCurve.p1;
                e->flightCurve.cp0 = e->flightCurve.p0 + sf::Vector2f(0, 140);
                e->flightCurve.p1 = sf::Vector2f(random(20, 300), 260);
                e->flightCurve.cp1 = e->flightCurve.p1 - sf::Vector2f(0, 140);
            }
            enemyWaveState = 2;
            break;
        case 2:
            foreach(Enemy, e, enemyList)
                if (e->flightCurve.delta != 1.0f)
                    return;
            
            foreach(Enemy, e, enemyList)
                e->destroy();
            enemyStartPosition = sf::Vector2f(random(20, 300), -20);
            
            enemyWaveState = 0;
            enemySpawnCount = 10;
            break;
        }
    }
    
    virtual void postRender(sf::RenderTarget& window)
    {
        window.draw(logoSprite);
        if (Clock.getElapsedTime().asMilliseconds() % 1000 < 500)
            window.draw(insertCoinSprite);
    }
};

void mainloop(sf::RenderWindow& window)
{
    //new BreEnemy();
    new mainMenu();
    
    StarBackground background;
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
        
        foreach(GameEntity, e, entityList)
        {
            e->update();
        }

        // Clear the window
        window.clear(sf::Color(0, 0, 0));
        background.render(window);
        foreach(GameEntity, e, entityList)
            e->preRender(window);
        foreach(GameEntity, e, entityList)
            e->render(window);
        foreach(GameEntity, e, entityList)
            e->postRender(window);
        
        // Display things on screen
        window.display();
    }
}

int main()
{
    srand(time(NULL));

    // Create the window of the application
    const int gameWidth = 320*3;
    const int gameHeight = 240*3;

    sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "iMvaders!", sf::Style::None);
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);
    window.setView(sf::View(sf::Vector2f(160,120), sf::Vector2f(320, 240)));

    logoTexture.loadFromFile("resources/iMvader.png");
    bulletTexture.loadFromFile("resources/bullet.png");
    bre1Texture.loadFromFile("resources/bre1.png");
    bre2Texture.loadFromFile("resources/bre2.png");
    insertCoinTexture.loadFromFile("resources/insertCoin.png");
    invaderTexture.loadFromFile("resources/MakerBotLogoMini.png");
    invaderShieldedTexture.loadFromFile("resources/MakerBotLogoMiniShielded.png");
    playerTexture.loadFromFile("resources/m.png");

    mainloop(window);

    return EXIT_SUCCESS;
}
