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
sf::Texture robotTexture;
sf::Texture insertCoinTexture;
sf::Texture ultimakerTexture[9];
sf::Texture numberTexture[10];
sf::Texture invaderTexture;
sf::Texture invaderShieldedTexture;
sf::Texture playerTexture;
sf::Clock Clock;

#include "explosion.h"
#include "bullet.h"
#include "player.h"
#include "enemy.h"
#include "bre.h"

PlayerController playerController[2];

class GameRound : public GameEntity
{
private:
    PVector<EnemyGroup> groupList;
    int diveCountdown;
public:
    GameRound()
    {
        enemyDirection = 0.2;
        enemyOffset = 0;
        diveCountdown = random(100, 300);
        
        EnemyGroup* g = new EnemyGroup();
        groupList.push_back(g);
        for(int n=0; n<8; n++)
            g->add(sf::Vector2f(160 - 4 * 20 + n * 20, 50))->giveShield();
        g = new EnemyGroup();
        groupList.push_back(g);
        for(int n=0; n<10; n++)
            g->add(sf::Vector2f(160 - 5 * 20 + n * 20, 70));
        g = new EnemyGroup();
        groupList.push_back(g);
        for(int n=0; n<4; n++)
            g->add(sf::Vector2f(160 - 5 * 20 + n * 20, 90));
        g = new EnemyGroup();
        groupList.push_back(g);
        for(int n=0; n<4; n++)
            g->add(sf::Vector2f(160 + 4 * 20 - n * 20, 90));
    }
    virtual ~GameRound() {}
    
    virtual void update()
    {
        if (enemyOffset > 30)
            enemyDirection = -fabs(enemyDirection);
        if (enemyOffset < -30)
            enemyDirection = fabs(enemyDirection);
        enemyOffset += enemyDirection;
        
        bool allowDive = true;
        bool allowFlyIn = true;
        foreach(EnemyGroup, g, groupList)
        {
            if (!g->isAll(ES_CenterField))
            {
                allowDive = false;
                if (!g->isAll(ES_Outside))
                    allowFlyIn = false;
            }
        }
        if (groupList.size() < 1)
        {
            //Destroy ourselves if all our groups are destroyed, to indicate the round is done.
            destroy();
            return;
        }
        
        if (allowFlyIn)
        {
            P<EnemyGroup> g = groupList[rand() % groupList.size()];
            if (g->isAll(ES_Outside))
                //g->flyIn(sf::Vector2f(random(0, 320), -20));
                g->flyIn(sf::Vector2f(random(0, 320), -20), sf::Vector2f(160, 160));
        }
        
        if (allowDive)
        {
            if (diveCountdown)
            {
                diveCountdown--;
            }
            else
            {
                diveCountdown = random(100, 300);
                P<EnemyGroup> g = groupList[rand() % groupList.size()];
                g->dive(sf::Vector2f(random(20, 300), 300));
            }
        }
        //foreach(Enemy, e, enemyList)
        //    e->flightCurve.p1 += sf::Vector2f(enemyDirection, 0);
    }
};

class GameState : public GameEntity
{
private:
    P<PlayerCraft> player;
    P<GameRound> round;
    int lives;
public:
    GameState()
    {
        lives = 4;
        //Destroy all objects except ourselves.
        foreach(GameEntity, e, entityList)
            if (e != this)
                e->destroy();
    }
    virtual ~GameState() {}
    
    virtual void update()
    {
        if (!player)
        {
            if (lives)
            {
                lives --;
                player = new PlayerCraft(&playerController[0]);
            }
            else
            {
                //foreach(GameEntity, e, entityList)
                //    e->destroy();
                //new MainMenu();
                return;
            }
        }
            
        if (!round)
            round = new GameRound();
    }
    
    virtual void postRender(sf::RenderTarget& window)
    {
        sf::Sprite life;
        life.setTexture(playerTexture);
        life.setScale(0.5, 0.5);
        life.setOrigin(playerTexture.getSize().x / 2, playerTexture.getSize().y / 2);
        life.setColor(sf::Color(255,255,255,192));
        for(int n=0; n<lives; n++)
        {
            life.setPosition(10 + 13 * n, 230);
            window.draw(life);
        }
    }
};

class MainMenu : public GameEntity
{
public:
    sf::Sprite logoSprite;
    sf::Sprite insertCoinSprite;
    P<EnemyGroup> enemyGroup;
    bool startGame;
    
    MainMenu()
    {
        startGame = false;
        
        logoSprite.setTexture(logoTexture);
        logoSprite.setOrigin(logoTexture.getSize().x / 2, 0);
        logoSprite.setPosition(160, 40);
        
        insertCoinSprite.setTexture(insertCoinTexture);
        insertCoinSprite.setOrigin(insertCoinTexture.getSize().x / 2, 0);
        insertCoinSprite.setPosition(160, 200);
        
        enemyGroup = new EnemyGroup();
        for(unsigned int n=0; n<10; n++)
            enemyGroup->add(sf::Vector2f(160+4*20 - n * 20, 100));
    }
    
    virtual ~MainMenu()
    {
    }
    
    virtual void update()
    {
        if (enemyGroup->isAll(ES_CenterField))
            enemyGroup->dive(sf::Vector2f(random(20, 300), 260));
        if (enemyGroup->isAll(ES_Outside))
            enemyGroup->flyIn(sf::Vector2f(random(0, 320), -20));
        
        if (!startGame)
        {
            if (playerController[0].fire())
                startGame = true;
        }
        if (startGame)
        {
            new GameState();
        }
    }
    
    virtual void postRender(sf::RenderTarget& window)
    {
        window.draw(logoSprite);
        if (startGame)
        {
            if (Clock.getElapsedTime().asMilliseconds() % 200 < 100)
                window.draw(insertCoinSprite);
        }
        else
        {
            if (Clock.getElapsedTime().asMilliseconds() % 1000 < 500)
                window.draw(insertCoinSprite);
        }
    }
};

void mainloop(sf::RenderWindow& window)
{
    //new BreEnemy();
    new MainMenu();
    
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
        {
#ifdef DEBUG
            if (e->collisionRadius > 0.0)
            {
                sf::CircleShape circle(e->collisionRadius, 30);
                circle.setOrigin(e->collisionRadius, e->collisionRadius);
                circle.setPosition(e->sprite.getPosition());
                circle.setFillColor(sf::Color::Transparent);
                circle.setOutlineColor(sf::Color(255,255,255,128));
                circle.setOutlineThickness(1);
                window.draw(circle);
            }
#endif
            e->preRender(window);
        }
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
    robotTexture.loadFromFile("resources/robot.png");
    ultimakerTexture[0].loadFromFile("resources/letter_u.png");
    ultimakerTexture[1].loadFromFile("resources/letter_l.png");
    ultimakerTexture[2].loadFromFile("resources/letter_t.png");
    ultimakerTexture[3].loadFromFile("resources/letter_i.png");
    ultimakerTexture[4].loadFromFile("resources/letter_m.png");
    ultimakerTexture[5].loadFromFile("resources/letter_a.png");
    ultimakerTexture[6].loadFromFile("resources/letter_k.png");
    ultimakerTexture[7].loadFromFile("resources/letter_e.png");
    ultimakerTexture[8].loadFromFile("resources/letter_r.png");
    numberTexture[0].loadFromFile("resources/num0.png");
    numberTexture[1].loadFromFile("resources/num1.png");
    numberTexture[2].loadFromFile("resources/num2.png");
    numberTexture[3].loadFromFile("resources/num3.png");
    numberTexture[4].loadFromFile("resources/num4.png");
    numberTexture[5].loadFromFile("resources/num5.png");
    numberTexture[6].loadFromFile("resources/num6.png");
    numberTexture[7].loadFromFile("resources/num7.png");
    numberTexture[8].loadFromFile("resources/num8.png");
    numberTexture[9].loadFromFile("resources/num9.png");
    insertCoinTexture.loadFromFile("resources/insertCoin.png");
    invaderTexture.loadFromFile("resources/MakerBotLogoMini.png");
    invaderShieldedTexture.loadFromFile("resources/MakerBotLogoMiniShielded.png");
    playerTexture.loadFromFile("resources/m.png");

    mainloop(window);

    return EXIT_SUCCESS;
}
