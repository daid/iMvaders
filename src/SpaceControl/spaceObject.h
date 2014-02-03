#ifndef SPACE_OBJECT_H
#define SPACE_OBJECT_H

#include <SFML/Graphics.hpp>
#include "Updatable.h"

class SpaceObject;
extern PVector<SpaceObject> spaceObjectList;

class Planet;
class SpaceObject: public Updatable
{
public:
    enum Physics
    {
        Fixed,
        Newtonian,
        Orbit
    } physics;

    bool alwaysVisible;
    sf::Sprite sprite;
    //Newtonian physics
    sf::Vector2f velocity;
    float angularVelocity;
    //Orbit physics
    P<Planet> orbitTarget;
    float orbitDistance;
    float orbitAngle;
    bool orbitPrograde;

    SpaceObject();
    virtual ~SpaceObject() {}
    
    virtual void update(float delta);
    
    virtual void renderOnRadar(sf::RenderTarget& window)
    {
        window.draw(sprite);
    }
    
    void setOrbit(Planet* target, float distance, float angle);
    
    sf::Vector2f getPosition() const
    {
        return sprite.getPosition();
    }
    
    sf::Vector2f predictPositionAtDelta(float delta);
    
    void setPosition(sf::Vector2f v)
    {
        sprite.setPosition(v);
    }

    float getRotation() const
    {
        return sprite.getRotation();
    }
    
    void setRotation(float angle)
    {
        sprite.setRotation(angle);
    }
    
    virtual bool takeDamage(float amount) { return false; }
};

#endif//SPACE_OBJECT_H
