#ifndef NUKE_H
#define NUKE_H

#include <SFML/Audio.hpp>

#include "gameEntity.h"
#include "Collisionable.h"

class Nuke: public GameEntity, public Collisionable
{
private:
    sf::Vector2f velocity;
    float size;
public:
    Nuke(sf::Vector2f position, sf::Vector2f velocity, float size);
    
    virtual void update(float delta);
    
    virtual void render(sf::RenderTarget& window);
    
    virtual void collision(Collisionable* other);
    
    void explode();
};

#endif//NUKE_H
