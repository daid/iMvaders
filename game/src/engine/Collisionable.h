#ifndef COLLISIONABLE_H
#define COLLISIONABLE_H

#include "P.h"
#include "Box2D/Box2D.h"

enum ECollisionType
{
    CT_Circle,
    CT_Line
};

class Collisionable;
class CollisionManager
{
public:
    static void initialize();
    static void handleCollisions(float delta);
private:
    static b2World* world;
    
    friend class Collisionable;
    friend class CollisionDebugDraw;
};

class Collisionable: public virtual PObject
{
private:
    sf::Vector2f position;
    float rotation;
    b2Body* body;
public:
    Collisionable(float radius);
    Collisionable(sf::Vector2f boxSize, sf::Vector2f boxOrigin = sf::Vector2f(0, 0));
    virtual ~Collisionable();
    virtual void collision(Collisionable* target);
    
    void setCollisionRadius(float radius);
    void setCollisionBox(sf::Vector2f boxSize, sf::Vector2f boxOrigin = sf::Vector2f(0, 0));
    void setCollisionShape(std::vector<sf::Vector2f> shape);
    
    void setPosition(sf::Vector2f v);
    sf::Vector2f getPosition();
    void setRotation(float angle);
    float getRotation();
    
    friend class CollisionManager;
};

#endif // COLLISIONABLE_H
