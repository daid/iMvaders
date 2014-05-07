#ifndef DIGITIZER_H
#define DIGITIZER_H

#include "gameEntity.h"
#include "Collisionable.h"
#include "textureManager.h"
#include "explosion.h"
#include "scriptInterface.h"

class DigitizerLaser;
class Digitizer: public GameEntity, public Collisionable
{
protected:
    int state;
    int health;
    static const int maxHealth = 10;
    P<DigitizerLaser> laser[2];
public:
    Digitizer();
    virtual ~Digitizer();
    virtual void update(float delta);
    virtual void collision(Collisionable* target);
    
    virtual void destroy()
    {
        laser[0] = NULL;
        laser[1] = NULL;
        GameEntity::destroy();
    }
    
    virtual void render(sf::RenderTarget& window);
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
};

class DigitizerLaser: public GameEntity, public Collisionable
{
    P<Digitizer> owner;
    float activateDelay;
    float deactivateDelay;
    static const float activateDelayMax = 2.0;
    static const float deactivateDelayMax = 1.0;
public:
    DigitizerLaser(P<Digitizer> owner);
    virtual ~DigitizerLaser() {}

    virtual void update(float delta);
    virtual void render(sf::RenderTarget& window);
    virtual void destroy()
    {
        owner = NULL;
        GameEntity::destroy();
    }

    virtual void collision(Collisionable* other);
};

#endif

