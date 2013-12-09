#include "gameState.h"
#include "EnemyGroup.h"
#include "bre.h"
#include "textDraw.h"
#include "postProcessManager.h"
#include "scoreManager.h"
#include "mainMenu.h"

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

class GameOverState : public GameEntity
{
    float gameOverDelay;
    static const float gameOverWait = 5.0;
public:
    GameOverState()
    {
        gameOverDelay = gameOverWait;
    }

    virtual void update(float delta)
    {
        if (gameOverDelay < gameOverWait - 1.0 && (playerController[0].fire() || playerController[1].fire()))
            gameOverDelay = 0;
        if (gameOverDelay > 0)
            gameOverDelay -= delta;
        else
        {
            //Clean out the game world.
            foreach(GameEntity, e, entityList)
                e->destroy();
            score.reset();
            new MainMenu();
        }
    }

    virtual void postRender(sf::RenderTarget& window)
    {
        drawText(window, 160, 120, "GAME OVER", align_center);
    }
};

GameState::GameState(int playerCount)
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
GameState::~GameState() {}

void GameState::update(float delta)
{
    bool gameOver = true;
    for(int n=0; n<playerCount; n++)
    {
        if (player[n])
        {
            gameOver = false;
        }
        else
        {
            if (lives[n])
            {
                lives[n] --;
                player[n] = new PlayerCraft(&playerController[n], n);
                postProcessorManager.triggerPostProcess("pixel", 1.0);
                gameOver = false;
            }
        }
    }
    if (gameOver)
    {
        destroy();
        new GameOverState();
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

void GameState::postRender(sf::RenderTarget& window)
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
