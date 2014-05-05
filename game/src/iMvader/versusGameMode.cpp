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
            setPosition(sf::Vector2f(-20, random(40, 200)));
            setVelocity(sf::Vector2f(random(-1, 1), random(-2, 2)) * 3.0f + sf::Vector2f( 25.0f, 0.0f));
        }else{
            setPosition(sf::Vector2f(340, random(40, 200)));
            setVelocity(sf::Vector2f(random(-1, 1), random(-2, 2)) * 3.0f + sf::Vector2f(-155.0f, 0.0f));
        }
    }
    virtual ~VersusOldPrinter() {}
    
    virtual void collision(Collisionable* other)
    {
        GameEntity* e = dynamic_cast<GameEntity*>(other);
        if (e && !dynamic_cast<OldPrinter*>(other))
        {
            e->takeDamage(getPosition(), 0, 1);
            e->takeDamage(getPosition(),-1, 1);
        }
    }

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
    {
        health -= damageAmount;
        if (damageType >= 0)
            applyImpulse(position, sf::Vector2f(0, 13));
        else
            applyImpulse(position, sf::Vector2f(0,-13));
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
    if (printerSpawnDelay > 0.0)
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
                P<PlayerController> pc = engine->getObject("playerController1");
                if (n)
                    pc = engine->getObject("playerController2");
                player[n] = new PlayerCraft(*pc, &playerInfo[n], (n == 0) ? 0 : -1);
                gameOver = false;
            }else{
                gameOver = true;
            }
        }
    }

    if (gameOver)
    {
        if (victoryDelay > 0.0)
        {
            victoryDelay -= delta;
        }else{
            foreach(GameEntity, e, entityList)
                e->destroy();
            new MainMenu();
        }
    }
}

void VersusGameState::render(sf::RenderTarget& window)
{
    sf::Sprite life;
    for(int p=0; p<MAX_PLAYERS; p++)
    {
        if (p == 0)
        {
            textureManager.setTexture(life, "player1");
            life.setScale(0.5, 0.5);
            life.setColor(sf::Color(255,255,255,192));
        }
        else
        {
            textureManager.setTexture(life, "BasicEnemy", 0);
            life.setScale(0.6, 0.6);
            life.setColor(sf::Color(212,0,0,192));
        }
        for(int n=0; n<playerInfo[p].lives; n++)
        {
            life.setPosition(20 + 13 * n, 230 - p * 10);
            window.draw(life);
        }
    }
}
