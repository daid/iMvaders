#ifndef OLDPRINTERS_H
#define OLDPRINTERS_H

#include "gameEntity.h"
#include "Collisionable.h"

class OldPrinter : public GameEntity, public Collisionable
{
protected:
    int health;
    static const int maxHealth = 10;
public:
    OldPrinter();
    virtual ~OldPrinter() {}

    virtual void update(float delta);
    
    virtual void render(sf::RenderTarget& window);

    virtual void collision(Collisionable* other);
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);};

#endif//OLDPRINTERS_H
