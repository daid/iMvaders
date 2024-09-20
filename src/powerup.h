#ifndef POWERUP_H
#define POWERUP_H

#include "engine.h"
#include "gameEntity.h"
#include "collisionable.h"


class Powerup: public GameEntity, public Collisionable
{
private:
    float speed;
public:
    Powerup();
    virtual ~Powerup();
    virtual void update(float delta) override;

    virtual void render(sp::RenderTarget& window) override;
    
    virtual void collide(Collisionable* target, float force) override;
};

#endif//POWERUP_H

