#ifndef COLLISIONABLE_H
#define COLLISIONABLE_H

#include "P.h"

class Collisionable;
extern PVector<Collisionable> collisionableList;
class Collisionable: public virtual PObject
{
private:
    sf::Vector2f position;
    float collisionRadius;
public:
    Collisionable(float radius);
    virtual ~Collisionable();
    virtual void collision(Collisionable* target);
    
    void setCollisionRadius(float radius);
    void setPosition(sf::Vector2f v);
    sf::Vector2f getPosition();


    static void handleCollisions();
};

#endif // COLLISIONABLE_H
