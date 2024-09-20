#include <string.h>
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
#include "main.h"

class HighscoreEntry : public GameEntity
{
public:
    glm::ivec2 pos[MAX_PLAYERS];
    float keyDelay[MAX_PLAYERS];
    char name[MAX_PLAYERS][4];
    bool done[MAX_PLAYERS];
    int playerCount;
    float timeout;
    
    HighscoreEntry(int playerCount)
    : GameEntity(hudLayer), playerCount(playerCount)
    {
        for(int p=0; p<MAX_PLAYERS; p++)
        {
            keyDelay[p] = 1.0;
            done[p] = false;
        }
        memset(name, 0, sizeof(name));
        timeout = 120.0;
    }

    virtual void update(float delta) override
    {
        P<PlayerController> pc[MAX_PLAYERS];
        for(int n=0; n<MAX_PLAYERS; n++)
            pc[n] = engine->getObject("playerController" + string(n + 1));
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
        
        if (timeout > 0)
        {
            timeout -= delta;
        }else{
            name[0][0] = '\0';
            name[1][0] = '\0';
            ready = true;
        }

        if (ready)
        {
            //Clean out the game world.
            foreach(GameEntity, e, entityList)
                e->destroy();
            
            P<ScoreManager> score = engine->getObject("score");
            string finalName = name[0];
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

    virtual void render(sp::RenderTarget& window) override
    {
        drawText(window, 160, 25, "NEW HIGHSCORE", align_center);
        drawText(window, 160, 40, "ENTER YOUR NAME", align_center);
        string finalName = name[0];
        for(int p=1; p<playerCount; p++)
        {
            finalName += " ";
            finalName += name[p];
        }
        drawText(window, 160, 60, finalName, align_center);

        Sprite player;
        for(int p=0; p<playerCount; p++)
        {
            if (done[p])
                continue;
            if ((p % 2) == 0)
                spriteManager.setTexture(player, "player1");
            else
                spriteManager.setTexture(player, "player2");
            player.setPosition(110 + pos[p].x * 20, 85 + pos[p].y * 20);
            player.draw(window);
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
    static constexpr float gameOverWait = 5.0;
    int playerCount;
public:
    GameOverState(int playerCount)
    : GameEntity(hudLayer), playerCount(playerCount)
    {
        gameOverDelay = gameOverWait;
    }

    virtual void update(float delta) override
    {
        bool fire_pressed = false;
        for(int n=0; n<MAX_PLAYERS; n++)
        {
            P<PlayerController> pc = engine->getObject("playerController" + string(n + 1));
            if (pc->button(fireButton))
                fire_pressed = true;
        }
    
        if (gameOverDelay < gameOverWait - 1.0f && fire_pressed)
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

    virtual void render(sp::RenderTarget& window) override
    {
        P<ScoreManager> score = engine->getObject("score");
        drawText(window, 160, 120, "GAME OVER", align_center);
        drawText(window, 160, 140, "SCORE " + string(score->get()), align_center);
    }
};

GameState::GameState(int playerCount)
: GameEntity(hudLayer)
{
    stageNr = 0;
    LOG(INFO) << "Player count at start: " << playerCount;
    for(int n=0; n<MAX_PLAYERS; n++)
    {
        playerInfo[n].active = n < playerCount;
        playerInfo[n].lives = 4;
        playerInfo[n].nukes = 1;
    }
    if (playerCount < MAX_PLAYERS)
        extra_player_spawn_delay = 10.0;
    else
        extra_player_spawn_delay = 0;
    startStageDelay = 2.0;
    reviveDelay = reviveTimeout;
    //Destroy all objects except ourselves.
    foreach(GameEntity, e, entityList)
        if (e != this)
            e->destroy();
    
    script = new ScriptObject("stage.lua");
}
GameState::~GameState() {}

void GameState::update(float delta)
{
    if (extra_player_spawn_delay > 0.0f)
    {
        extra_player_spawn_delay -= delta;
        for(int p=0; p<MAX_PLAYERS; p++)
        {
            if (playerInfo[p].active)
                continue;
            P<PlayerController> pc = engine->getObject("playerController" + string(p + 1));
            if (pc->button(fireButton))
            {
                playerInfo[p].active = true;
            }
        }
    }

    {
        int deadPlayer = -1;
        int livePlayer = -1;
        for(int p=0; p<MAX_PLAYERS; p++)
        {
            if (!playerInfo[p].active)
                continue;
            if (playerInfo[p].lives < 1 && !player[p])
                deadPlayer = p;
            if (playerInfo[p].lives > 0 && player[p])
                livePlayer = p;
        }
        
        if (deadPlayer > -1 && livePlayer > -1)
        {
            P<PlayerController> pc = engine->getObject("playerController" + string(livePlayer + 1));
        
            if (!pc->up() && !pc->down() && !pc->left() && !pc->right() && !pc->button(fireButton))
            {
                reviveDelay -= delta;
                if (reviveDelay < 0.0f)
                {
                    playerInfo[livePlayer].lives --;
                    playerInfo[deadPlayer].lives ++;
                    reviveDelay = reviveTimeout;
                }
            }else{
                reviveDelay = reviveTimeout;
            }
        }
    }

    bool gameOver = true;
    for(int n=0; n<MAX_PLAYERS; n++)
    {
        if (!playerInfo[n].active)
            continue;
        if (player[n])
        {
            gameOver = false;
        }
        else
        {
            if (playerInfo[n].lives)
            {
                LOG(INFO) << "Spawned player: " << n;
                playerInfo[n].lives --;
                P<PlayerController> pc = engine->getObject("playerController" + string(n + 1));
                player[n] = new PlayerCraft(*pc, &playerInfo[n], n % 2);
                gameOver = false;
            }
        }
    }
    if (escape_key.getDown())
    {
        gameOver = true;
    }

    if (gameOver)
    {
        if (script)
        {
            script->destroy();
        }
        destroy();
        int player_count = 0;
        for(int n=0; n<MAX_PLAYERS; n++)
            if (playerInfo[n].active)
                player_count += 1;
        new GameOverState(player_count);
    }
}

void GameState::render(sp::RenderTarget& window)
{
    Sprite nukeIcon;
    
    spriteManager.setTexture(nukeIcon, "robot");
    nukeIcon.setScale(0.12, 0.12);
    
    Sprite life;
    for(int p=0; p<MAX_PLAYERS; p++)
    {
        if (!playerInfo[p].active)
            continue;
        if ((p % 2) == 0)
            spriteManager.setTexture(life, "player1");
        else
            spriteManager.setTexture(life, "player2");
        life.setScale(0.5, 0.5);
        life.setColor({255,255,255,192});
        for(int n=0; n<playerInfo[p].lives; n++)
        {
            life.setPosition(20 + 13 * n, 230 - p * 10);
            life.draw(window);
        }

        if ((p % 2) == 0)
            nukeIcon.setColor({24, 161, 212, 255});
        else
            nukeIcon.setColor({231, 24, 118, 255});
        for(int n=0; n<playerInfo[p].nukes; n++)
        {
            nukeIcon.setPosition(60 + 10 * n, 230 - p * 10);
            nukeIcon.draw(window);
        }
    }
    
    if (reviveDelay < reviveTimeout - 0.5f)
    {
        RectangleShape reviveBarBG({40, 8});
        reviveBarBG.setFillColor({0,0,0,0});
        reviveBarBG.setOutlineColor({128, 128, 128, 128});
        reviveBarBG.setOutlineThickness(1);
        reviveBarBG.setPosition(15, 200);
        reviveBarBG.draw(window);

        RectangleShape reviveBar({40 * ((reviveTimeout - 0.5f) - reviveDelay) / (reviveTimeout - 0.5f), 8});
        reviveBar.setFillColor({24, 161, 212, 128});
        reviveBar.setPosition(15, 200);
        reviveBar.draw(window);
    }

    if (P<ScoreManager>(engine->getObject("score"))->get() < 1 && !playerInfo[1].active)
    {
        if (extra_player_spawn_delay > 0.0f && fmodf(extra_player_spawn_delay, 1.0) < 0.5f)
            drawText(window, 300, 220, "Press fire to join", align_right, 0.7);
    }else{
        drawText(window, 300, 220, string(P<ScoreManager>(engine->getObject("score"))->get()), align_right);
    }
}
