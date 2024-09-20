#ifndef NUKE_H
#define NUKE_H

#include "engine.h"
#include "gameEntity.h"
#include "collisionable.h"


class Nuke: public GameEntity, public Collisionable
{
private:
    glm::vec2 velocity{};
    float size;
    int owner;
public:
    Nuke(glm::vec2 position, glm::vec2 velocity, float size, int owner);
    virtual ~Nuke() {}
    
    virtual void update(float delta) override;
    
    virtual void render(sp::RenderTarget& window) override;
    
    virtual void collide(Collisionable* other, float force) override;
    
    void explode();
};

#endif//NUKE_H
