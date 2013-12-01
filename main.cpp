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

PlayerController playerController[2];

class GameStage : public GameEntity
{
private:
    PVector<EnemyGroup> groupList;
    float diveCountdown;
    float enemyOffset;
    float enemyDirection;
public:
    GameStage()
    {
        enemyDirection = 20.0;
        enemyOffset = 0;
        diveCountdown = random(2.0, 5.0);

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

    virtual void update(float delta)
    {
        if (enemyOffset > 30)
            enemyDirection = -fabs(enemyDirection);
        if (enemyOffset < -30)
            enemyDirection = fabs(enemyDirection);
        enemyOffset += enemyDirection * delta;

        bool allowDive = true;
        bool allowFlyIn = true;
        foreach(EnemyGroup, g, groupList)
        {
            g->setOffset(enemyOffset);
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
            if (diveCountdown > 0)
            {
                diveCountdown -= delta;
            }
            else
            {
                diveCountdown = random(2.0, 5.0);
                P<EnemyGroup> g = groupList[rand() % groupList.size()];
                g->dive(sf::Vector2f(random(20, 300), 300));
            }
        }
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

    virtual void update(float delta)
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
    P<PlayerCraft> player[MAX_PLAYERS];
    P<GameEntity> stage;
    int lives[MAX_PLAYERS];
    int stageNr;
    int playerCount;
    float startStageDelay;
public:
    GameState(int playerCount)
    : playerCount(playerCount)
    {
        stageNr = 0;
        for(int n=0; n<playerCount; n++)
            lives[n] = 4;
        startStageDelay = 2.0;
        //Destroy all objects except ourselves.
        foreach(GameEntity, e, entityList)
            if (e != this)
                e->destroy();
    }
    virtual ~GameState() {}

    virtual void update(float delta)
    {
        for(int n=0; n<playerCount; n++)
        {
            if (!player[n])
            {
                if (lives[n])
                {
                    lives[n] --;
                    player[n] = new PlayerCraft(&playerController[n], n);
                    postProcessorManager.triggerPostProcess("pixel", 1.0);
                }
                else
                {
                    //foreach(GameEntity, e, entityList)
                    //    e->destroy();
                    //new MainMenu();
                    //return;
                }
            }
        }

        if (!stage)
        {
            if (startStageDelay > 0)
            {
                startStageDelay -= delta;
            }
            else
            {
                stageNr++;
                if (stageNr % 3 == 0)
                    stage = new BreStage();
                else
                    stage = new GameStage();
                startStageDelay = 2.0;
            }
        }
    }

    virtual void postRender(sf::RenderTarget& window)
    {
        sf::Sprite life;
        for(int p=0; p<playerCount; p++)
        {
            if (p == 0)
                textureManager.setTexture(life, "player1");
            else
                textureManager.setTexture(life, "player2");
            life.setScale(0.5, 0.5);
            life.setColor(sf::Color(255,255,255,192));
            for(int n=0; n<lives[p]; n++)
            {
                life.setPosition(10 + 13 * n, 230 - p * 10);
                window.draw(life);
            }
        }
        
        drawText(window, 310, 220, to_string(score.get()), align_right);

        if (!stage)
        {
            if (startStageDelay > 1.0)
                drawText(window, 160, 120 - (startStageDelay - 1.0) * 120, "STAGE " + to_string(stageNr+1));
            else
                drawText(window, 160, 120, "STAGE " + to_string(stageNr+1));
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

    virtual void update(float delta)
    {
        if (enemyGroup->isAll(ES_CenterField))
            enemyGroup->dive(sf::Vector2f(random(20, 300), 260));
        if (enemyGroup->isAll(ES_Outside))
            enemyGroup->flyIn(sf::Vector2f(random(0, 320), -20));

        if (!startGame)
        {
            if (playerController[0].fire())
            {
                new GameState(1);
            }
            if (playerController[1].fire())
            {
                new GameState(2);
            }
        }
        if (startGame)
        {
        }
    }

    virtual void postRender(sf::RenderTarget& window)
    {
        window.draw(logoSprite);
        drawText(window, 160, 120, "HIGH SCORE");
        for(int i=0; i<score.highscoreListSize; i++)
        {
            drawText(window, 160, 120 + 16 * (i+1), score.getHighscoreName(i) + " " + to_string(score.getHighScore(i)));
        }
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
    new MainMenu();

    sf::Clock frameTimeClock;
    new StarBackground();
    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed)
                printf("%i\n", event.key.code);
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
