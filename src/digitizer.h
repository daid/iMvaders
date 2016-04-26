#ifndef DIGITIZER_H
#define DIGITIZER_H

#include "engine.h"
#include "explosion.h"

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
    virtual void update(float delta) override;
    virtual void collide(Collisionable* target, float force) override;
    
    virtual void destroy() override
    {
        laser[0] = NULL;
        laser[1] = NULL;
        GameEntity::destroy();
    }
    
    virtual void render(sf::RenderTarget& window) override;
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount) override;
};

class DigitizerLaser: public GameEntity, public Collisionable
{
    P<Digitizer> owner;
    float activateDelay;
    float deactivateDelay;
    static constexpr float activateDelayMax = 2.0;
    static constexpr float deactivateDelayMax = 1.0;
public:
    DigitizerLaser(P<Digitizer> owner);
    virtual ~DigitizerLaser() {}

    virtual void update(float delta) override;
    virtual void render(sf::RenderTarget& window) override;
    virtual void destroy() override
    {
        owner = NULL;
        GameEntity::destroy();
    }

    virtual void collide(Collisionable* other, float force) override;
};

#endif

