#ifndef OLDPRINTERS_H
#define OLDPRINTERS_H

#include "engine.h"

class OldPrinter : public GameEntity, public Collisionable
{
protected:
    int health;
    static const int maxHealth = 10;
public:
    OldPrinter();
    virtual ~OldPrinter() {}

    virtual void update(float delta) override;
    
    virtual void render(sf::RenderTarget& window) override;

    virtual void collide(Collisionable* other, float force) override;
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount) override;
};

#endif//OLDPRINTERS_H
