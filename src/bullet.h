#ifndef BULLET_H
#define BULLET_H

#include "engine.h"
#include "gameEntity.h"
#include "collisionable.h"

class Bullet: public GameEntity, public Collisionable
{
private:
    float speed;
    int type;
public:
    Bullet(glm::vec2 position, int type, float angle, float speed = 180.0f);
    virtual ~Bullet() {}
    
    virtual void update(float delta) override;
    
    virtual void render(sp::RenderTarget& window) override;
    
    virtual void collide(Collisionable* other, float force) override;
};

#endif//BULLET_H
