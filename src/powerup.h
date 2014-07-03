#ifndef POWERUP_H
#define POWERUP_H

#include "gameEntity.h"
#include "Collisionable.h"

class Powerup: public GameEntity, public Collisionable
{
private:
    float speed;
public:
    Powerup();
    virtual ~Powerup();
    virtual void update(float delta);

    virtual void render(sf::RenderTarget& window);
    
    virtual void collision(Collisionable* target);
};

#endif//POWERUP_H

