#include "versusGameMode.h"
#include "textureManager.h"
#include "engine.h"
#include "oldPrinters.h"
#include "mainMenu.h"
#include "main.h"
#include "explosion.h"

class VersusOldPrinter : public OldPrinter
{
public:
    VersusOldPrinter()
    : OldPrinter()
    {
        if (random(0, 100) < 90)
        {
            setPosition(glm::vec2(-20, random(40, 200)));
            setVelocity(glm::vec2(random(-1, 1), random(-2, 2)) * 3.0f + glm::vec2( 25.0f, 0.0f));
        }else{
            setPosition(glm::vec2(340, random(40, 200)));
            setVelocity(glm::vec2(random(-1, 1), random(-2, 2)) * 3.0f + glm::vec2(-155.0f, 0.0f));
        }
    }
    virtual ~VersusOldPrinter() {}
    
    virtual void collide(Collisionable* other, float force) override
    {
        GameEntity* e = dynamic_cast<GameEntity*>(other);
        if (e && !dynamic_cast<OldPrinter*>(other))
        {
            e->takeDamage(getPosition(), 0, 1);
            e->takeDamage(getPosition(),-1, 1);
        }
    }

    virtual bool takeDamage(glm::vec2 position, int damageType, int damageAmount) override
    {
        health -= damageAmount;
        if (damageType >= 0)
            applyImpulse(position, glm::vec2(0, 13));
        else
            applyImpulse(position, glm::vec2(0,-13));
        if (health <= 0)
        {
            new Explosion(getPosition(), 8);
            destroy();
        }
        return true;
    }
};

VersusGameState::VersusGameState()
: GameEntity(hudLayer)
{
    for(int n=0; n<MAX_PLAYERS; n++)
    {
        playerInfo[n].lives = 4;
        playerInfo[n].nukes = 0;
    }
    //Destroy all objects except ourselves.
    foreach(GameEntity, e, entityList)
        if (e != this)
            e->destroy();
    
    for(unsigned int n=0; n<50; n++)
    {
        VersusOldPrinter* p = new VersusOldPrinter();
        p->setPosition(p->getPosition() + p->getVelocity() * float(n) * printerSpawnTime);
    }
    printerSpawnDelay = printerSpawnTime;
    victoryDelay = 5.0;
}

void VersusGameState::update(float delta)
{
    if (printerSpawnDelay > 0.0f)
        printerSpawnDelay -= delta;
    else
    {
        printerSpawnDelay += printerSpawnTime;
        new VersusOldPrinter();
    }

    bool gameOver = false;
    for(int n=0; n<MAX_PLAYERS; n++)
    {
        if (!player[n])
        {
            if (playerInfo[n].lives)
            {
                playerInfo[n].lives --;
                P<PlayerController> pc = engine->getObject("playerController" + string(n + 1));
                player[n] = new PlayerCraft(*pc, &playerInfo[n], (n == 0) ? 0 : -1);
                gameOver = false;
            }else{
                gameOver = true;
            }
        }
    }

    if (gameOver)
    {
        if (victoryDelay > 0.0f)
        {
            victoryDelay -= delta;
        }else{
            foreach(GameEntity, e, entityList)
                e->destroy();
            new MainMenu();
        }
    }
}

void VersusGameState::render(sp::RenderTarget& window)
{
    Sprite life;
    for(int p=0; p<MAX_PLAYERS; p++)
    {
        if (p == 0)
        {
            spriteManager.setTexture(life, "player1");
            life.setScale(0.5, 0.5);
            life.setColor({255,255,255,192});
        }
        else
        {
            spriteManager.setTexture(life, "BasicEnemy", 0);
            life.setScale(0.6, 0.6);
            life.setColor({212,0,0,192});
        }
        for(int n=0; n<playerInfo[p].lives; n++)
        {
            life.setPosition(20 + 13 * n, 230 - p * 10);
            life.draw(window);
        }
    }
}
