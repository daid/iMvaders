#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>


int scoreCount;
void addScore(int amount) { scoreCount += amount; }


sf::Clock Clock;

#include "vectorUtils.h"
#include "curve.h"
#include "background.h"
#include "gameEntity.h"
#include "textureManager.h"


#include "explosion.h"
#include "bullet.h"
#include "player.h"

#include "EnemyGroup.h"
#include "BasicEnemy.h"
#include "BurstShotEnemy.h"

#include "bre.h"

#include "Updatable.h"
#include "Renderable.h"
//#define DEBUG




int textWidth(const char* str)
{
    int ret = 0;
    while(*str)
    {
        int c = (*str++);
        if (c >= 'A' && c <= 'Z')
            ret += textureManager.getSpriteRect("abc", c-'A').width;
        else if (c >= '0' && c <= '9')
            ret += textureManager.getSpriteRect("nums", c-'0').width;
        else
            ret += 8;
    }
    return ret;
}

enum textAlign
{
    align_center = 0,
    align_left = 1,
    align_right = 2
};

void drawText(sf::RenderTarget& window, int x, int y, const char* str, textAlign align = align_center)
{
    sf::Sprite letter;
    if (align == align_center)
        x -= textWidth(str)/2;
    if (align == align_right)
        x -= textWidth(str);
    while(*str)
    {
        int c = (*str++);
        if (c >= 'A' && c <= 'Z')
        {
            c -= 'A';
            textureManager.setTexture(letter, "abc", c);
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            x += letter.getTextureRect().width + 1;
            window.draw(letter);
        }
        else if (c >= '0' && c <= '9')
        {
            textureManager.setTexture(letter, "nums", c-'0');
            letter.setOrigin(0, 0);
            letter.setPosition(x, y);
            x += letter.getTextureRect().width + 1;
            window.draw(letter);
        }
        else
        {
            x += 8;
        }
    }
}



PlayerController playerController[2];

class GameStage : public GameEntity
{
private:
    PVector<EnemyGroup> groupList;
    int diveCountdown;
    float enemyOffset;
    float enemyDirection;
public:
    GameStage()
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
    virtual ~GameStage() {}

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

class BreStage : public GameEntity
{
private:
    P<BreEnemy> bre;
public:
    BreStage()
    {
        bre = new BreEnemy();
    }
    virtual ~BreStage() {}

    virtual void update()
    {
        if (!bre)
        {
            //Destroy ourselves if bre is destroyed, to indicate the round is done.
            destroy();
            return;
        }
    }
};

class GameState : public GameEntity
{
private:
    P<PlayerCraft> player;
    P<GameEntity> stage;
    int lives;
    int stageNr;
    int startStageDelay;
public:
    GameState()
    {
        stageNr = 0;
        scoreCount = 0;
        lives = 4;
        startStageDelay = 120;
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

        if (!stage)
        {
            if (startStageDelay)
            {
                startStageDelay--;
            }
            else
            {
                stageNr++;
                if (stageNr % 3 == 0)
                    stage = new BreStage();
                else
                    stage = new GameStage();
                startStageDelay = 120;
            }
        }
    }

    virtual void postRender(sf::RenderTarget& window)
    {
        sf::Sprite life;
        textureManager.setTexture(life, "m");
        life.setScale(0.5, 0.5);
        life.setColor(sf::Color(255,255,255,192));
        for(int n=0; n<lives; n++)
        {
            life.setPosition(10 + 13 * n, 230);
            window.draw(life);
        }
        char buf[8];
        sprintf(buf, "%i", scoreCount);
        drawText(window, 310, 220, buf, align_right);

        if (!stage)
        {
            char buf[16];
            sprintf(buf, "STAGE %i", stageNr + 1);
            if (startStageDelay > 60)
                drawText(window, 160, 240 - startStageDelay * 2, buf);
            else
                drawText(window, 160, 120, buf);
        }
    }
};

class MainMenu : public GameEntity
{
public:
    sf::Sprite logoSprite;
    P<EnemyGroup> enemyGroup;
    bool startGame;

    MainMenu()
    {
        startGame = false;

        textureManager.setTexture(logoSprite, "iMvader");
        logoSprite.setPosition(160, 60);

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
        drawText(window, 160, 120, "HIGH SCORE");
        drawText(window, 160, 120 + 16 * 1, "DAV 1337");
        drawText(window, 160, 120 + 16 * 2, "DAV 1337");
        drawText(window, 160, 120 + 16 * 3, "DAV 1337");
        if (startGame)
        {
            if (Clock.getElapsedTime().asMilliseconds() % 200 < 100)
                drawText(window, 160, 200, "INSERT COIN");
        }
        else
        {
            if (Clock.getElapsedTime().asMilliseconds() % 1000 < 500)
                drawText(window, 160, 200, "INSERT COIN");
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

        foreach(Updatable, u, updatableList)
        {
            u->update();
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

        }
        foreach(Renderable,r,renderableList)
            r->preRender(window);
        foreach(Renderable,r,renderableList)
            r->render(window);
        foreach(Renderable,r,renderableList)
            r->postRender(window);

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

    mainloop(window);

    return EXIT_SUCCESS;
}
