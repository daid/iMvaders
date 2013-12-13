#include "gameState.h"
#include "EnemyGroup.h"
#include "bre.h"
#include "textDraw.h"
#include "postProcessManager.h"
#include "scoreManager.h"
#include "mainMenu.h"
#include "scriptInterface.h"
#include "powerupCarrier.h"

class GameStage : public GameEntity
{
private:
    P<ScriptObject> script;
    PVector<EnemyGroup> groupList;
    float diveCountdown;
    float enemyOffset;
    float enemyDirection;
public:
    GameStage()
    {
        script = new ScriptObject("resources/stage.lua");
    }
    virtual ~GameStage() {}

    virtual void update(float delta)
    {
        if (!script)
        {
            //Destroy ourselves if our script destroyed itself.
            destroy();
            return;
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
        if (gameOverDelay < gameOverWait - 1.0 && (playerController[0].button(fireButton) || playerController[1].button(fireButton)))
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
