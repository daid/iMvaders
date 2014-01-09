#include "stringUtils.h"
#include "gameState.h"
#include "EnemyGroup.h"
#include "textDraw.h"
#include "postProcessManager.h"
#include "scoreManager.h"
#include "mainMenu.h"
#include "scriptInterface.h"
#include "powerupCarrier.h"
#include "transmission.h"
#include "engine.h"

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
        P<PlayerController> pc1 = engine->getObject("playerController1");
        P<PlayerController> pc2 = engine->getObject("playerController2");
    
        if (gameOverDelay < gameOverWait - 1.0 && (pc1->button(fireButton) || pc2->button(fireButton)))
            gameOverDelay = 0;
        if (gameOverDelay > 0)
            gameOverDelay -= delta;
        else
        {
            //Clean out the game world.
            foreach(GameEntity, e, entityList)
                e->destroy();
            P<ScoreManager>(engine->getObject("score"))->reset();
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
    {
        playerInfo[n].lives = 4;
        playerInfo[n].nukes = 1;
    }
    startStageDelay = 2.0;
    //Destroy all objects except ourselves.
    foreach(GameEntity, e, entityList)
        if (e != this)
            e->destroy();
    
    script = new ScriptObject("resources/stage.lua");
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
            if (playerInfo[n].lives)
            {
                playerInfo[n].lives --;
                P<PlayerController> pc = engine->getObject("playerController1");
                if (n)
                    pc = engine->getObject("playerController2");
                player[n] = new PlayerCraft(*pc, &playerInfo[n], n);
                postProcessorManager.triggerPostProcess("pixel", 1.0);
                gameOver = false;
            }
        }
    }
    if (gameOver)
    {
        if (script) script->destroy();
        destroy();
        new GameOverState();
    }
}

void GameState::postRender(sf::RenderTarget& window)
{
    sf::Sprite nukeIcon;
    
    textureManager.setTexture(nukeIcon, "robot");
    nukeIcon.setScale(0.12, 0.12);
    
    sf::Sprite life;
    for(int p=0; p<playerCount; p++)
    {
        if (p == 0)
            textureManager.setTexture(life, "player1");
        else
            textureManager.setTexture(life, "player2");
        life.setScale(0.5, 0.5);
        life.setColor(sf::Color(255,255,255,192));
        for(int n=0; n<playerInfo[p].lives; n++)
        {
            life.setPosition(10 + 13 * n, 230 - p * 10);
            window.draw(life);
        }

        for(int n=0; n<playerInfo[p].nukes; n++)
        {
            nukeIcon.setPosition(50 + 10 * n, 230 - p * 10);
            window.draw(nukeIcon);
        }
    }
    
    drawText(window, 310, 220, to_string(P<ScoreManager>(engine->getObject("score"))->get()), align_right);
}
