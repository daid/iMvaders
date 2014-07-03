#ifndef REPLICATOR_Z18_H
#define REPLICATOR_Z18_H

#include "gameEntity.h"
#include "Collisionable.h"

class ReplicatorZ18Part;
class ReplicatorZ18 : public GameEntity
{
public:
    PVector<ReplicatorZ18Part> parts;
    
    ReplicatorZ18();
    virtual ~ReplicatorZ18() {}

    virtual void update(float delta);
    
    void setSpeed(float speed);
};

class ReplicatorZ18Part : public GameEntity, public Collisionable
{
    float speed;
    int health;
    float tinyExplosionDelay;
    static const int maxHealth = 1;
public:
    bool indestructible;
    P<ReplicatorZ18Part> neighbours[4];
    
    ReplicatorZ18Part(P<ReplicatorZ18> owner, int index);
    virtual ~ReplicatorZ18Part() {}
    
    virtual void destroy();

    virtual void update(float delta);
    virtual void render(sf::RenderTarget& window);

    virtual void collision(Collisionable* other);
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
    
    void setSpeed(float speed);
};

#endif//REPLICATOR_Z18_H
