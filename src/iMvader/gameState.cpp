#include <string.h>
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

class HighscoreEntry : public GameEntity
{
public:
    sf::Vector2i pos[MAX_PLAYERS];
    float keyDelay[MAX_PLAYERS];
    char name[MAX_PLAYERS][4];
    bool done[MAX_PLAYERS];
    int playerCount;
    
    HighscoreEntry(int playerCount)
    : playerCount(playerCount)
    {
        for(int p=0; p<MAX_PLAYERS; p++)
        {
            keyDelay[p] = 1.0;
            done[p] = false;
        }
        memset(name, 0, sizeof(name));
    }

    virtual void update(float delta)
    {
        P<PlayerController> pc[MAX_PLAYERS];
        pc[0] = engine->getObject("playerController1");
        pc[1] = engine->getObject("playerController2");
        bool ready = true;
        for(int p=0; p<playerCount; p++)
        {
            if (done[p])
                continue;
            ready = false;
            if (keyDelay[p] < 0)
            {
                if (pc[p]->right())
                {
                    pos[p].x += 1;
                    keyDelay[p] = 0.2;
                }
                if (pc[p]->left())
                {
                    pos[p].x -= 1;
                    keyDelay[p] = 0.2;
                }
                if (pc[p]->down())
                {
                    pos[p].y += 1;
                    keyDelay[p] = 0.2;
                }
                if (pc[p]->up())
                {
                    pos[p].y -= 1;
                    keyDelay[p] = 0.2;
                }
                
                if (pc[p]->button(fireButton))
                {
                    keyDelay[p] = 0.2;
                    char c = (pos[p].x + pos[p].y * 6) + 'A';
                    if (c <= 'Z' && strlen(name[p]) < 3)
                        name[p][strlen(name[p])] = c;
                    
                    if (pos[p].x == 3 && pos[p].y == 4)
                        name[p][strlen(name[p])-1] = '\0';
                    if (pos[p].x >= 4 && pos[p].y == 4)
                        done[p] = true;
                }
            }else{
                keyDelay[p] -= delta;
            }
            
            if(pos[p].x < 0)
                pos[p].x = 0;
            if(pos[p].x > 5)
                pos[p].x = 5;
            if(pos[p].y < 0)
                pos[p].y = 0;
            if(pos[p].y > 4)
                pos[p].y = 4;
        } 

        if (ready)
        {
            //Clean out the game world.
            foreach(GameEntity, e, entityList)
                e->destroy();
            
            P<ScoreManager> score = engine->getObject("score");
            std::string finalName = name[0];
            for(int p=1; p<playerCount; p++)
            {
                finalName += " ";
                finalName += name[p];
            }
            score->enterHighscore(playerCount, finalName);
            score->reset();
            new MainMenu();
        }
    }

    virtual void postRender(sf::RenderTarget& window)
    {
        drawText(window, 160, 25, "NEW HIGHSCORE", align_center);
        drawText(window, 160, 40, "ENTER YOUR NAME", align_center);
        std::string finalName = name[0];
        for(int p=1; p<playerCount; p++)
        {
            finalName += " ";
            finalName += name[p];
        }
        drawText(window, 160, 60, finalName, align_center);

        sf::Sprite player;
        for(int p=0; p<playerCount; p++)
        {
            if (done[p])
                continue;
            if (p == 0)
                textureManager.setTexture(player, "player1");
            else
                textureManager.setTexture(player, "player2");
            player.setPosition(110 + pos[p].x * 20, 85 + pos[p].y * 20);
            window.draw(player);
        }
        
        for(int c='A'; c<='Z'; c++)
        {
            int x = (c-'A') % 6;
            int y = (c-'A') / 6;
            char buf[2] = {char(c), 0};
            drawText(window, 110 + 20 * x, 80 + 20 * y, buf, align_center);
        }
        drawText(window, 200, 80 + 20 * 4, "DONE", align_center);
    }
};

class GameOverState : public GameEntity
{
    float gameOverDelay;
    static const float gameOverWait = 5.0;
    int playerCount;
public:
    GameOverState(int playerCount)
    : playerCount(playerCount)
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
            P<ScoreManager> score = engine->getObject("score");
            if (score->isHighscore(playerCount))
            {
                new HighscoreEntry(playerCount);
            }else{
                score->reset();
                new MainMenu();
            }
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
        new GameOverState(playerCount);
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
