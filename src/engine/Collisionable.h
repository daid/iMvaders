#ifndef COLLISIONABLE_H
#define COLLISIONABLE_H

#include "P.h"

class Collisionable;
extern PVector<Collisionable> collisionableList;
enum ECollisionType
{
    CT_Circle,
    CT_Line
};

class Collisionable: public virtual PObject
{
private:
    sf::Vector2f position;
    sf::Vector2f collisionLineVector;
    ECollisionType collisionType;
    float collisionRadius;
public:
    Collisionable(float radius);
    Collisionable(sf::Vector2f lineVector);
    virtual ~Collisionable();
    virtual void collision(Collisionable* target);
    
    void setCollisionRadius(float radius);
    void setCollisionLineVector(sf::Vector2f lineVector);
    void setPosition(sf::Vector2f v);
    sf::Vector2f getPosition();


    static void handleCollisions();
};

#endif // COLLISIONABLE_H
