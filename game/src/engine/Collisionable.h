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
    float rotation;
    b2Body* body;
    bool enablePhysics;
    bool staticPhysics;
    
    void createBody(b2Shape* shape);
public:
    Collisionable(float radius);
    Collisionable(sf::Vector2f boxSize, sf::Vector2f boxOrigin = sf::Vector2f(0, 0));
    Collisionable(const std::vector<sf::Vector2f>& shape);
    virtual ~Collisionable();
    virtual void collision(Collisionable* target);
    
    void setCollisionRadius(float radius);
    void setCollisionBox(sf::Vector2f boxSize, sf::Vector2f boxOrigin = sf::Vector2f(0, 0));
    void setCollisionShape(const std::vector<sf::Vector2f>& shape);
    void setCollisionPhysics(bool enablePhysics, bool staticPhysics);
    
    void setPosition(sf::Vector2f v);
    sf::Vector2f getPosition();
    void setRotation(float angle);
    float getRotation();
    void setVelocity(sf::Vector2f velocity);
    sf::Vector2f getVelocity();
    
    sf::Vector2f toLocalSpace(sf::Vector2f v);
    sf::Vector2f toWorldSpace(sf::Vector2f v);
    
    friend class CollisionManager;
};

#endif // COLLISIONABLE_H
