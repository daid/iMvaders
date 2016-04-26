#ifndef BULLET_H
#define BULLET_H

#include <SFML/Audio.hpp>

#include "engine.h"

class Bullet: public GameEntity, public Collisionable
{
private:
    float speed;
    int type;
public:
    Bullet(sf::Vector2f position, int type, float angle, float speed = 180.0f);
    virtual ~Bullet() {}
    
    virtual void update(float delta) override;
    
    virtual void render(sf::RenderTarget& window) override;
    
    virtual void collide(Collisionable* other, float force) override;
};

#endif//BULLET_H
