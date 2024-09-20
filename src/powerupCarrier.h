#ifndef POWERUP_CARRIER_H
#define POWERUP_CARRIER_H

#include "engine.h"
#include "gameEntity.h"
#include "collisionable.h"
#include "scriptInterface.h"


class PowerupCarrier: public GameEntity, public Collisionable
{
private:
    float speed;
    float direction;
public:
    ScriptCallback destroyed;
    
    PowerupCarrier();
    virtual ~PowerupCarrier();
    virtual void update(float delta) override;

    virtual bool takeDamage(glm::vec2 position, int damageType, int damageAmount) override;
    virtual void render(sp::RenderTarget& window) override;
    
    void setDirection(int dir);
};

#endif//POWERUP_CARRIER_H
