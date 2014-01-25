#ifndef PLANET_H
#define PLANET_H

#include "spaceObject.h"

class Planet;
extern PVector<Planet> planetList;

class Planet: public SpaceObject
{
    const static float G = 0.0000000000667;
    float mass;
public:
    float radius;

    Planet(int type, float radius, float density, sf::Vector2f position);
    
    virtual void update(float delta);
    
    float getRadius() const { return radius; }
    
    float calcOrbitVelocity(float distance) const;
    float calcOrbitTime(float distance) const;
    
    sf::Vector2f gravity(sf::Vector2f position) const;
    sf::Vector2f gravity(sf::Vector2f position, float deltaTime);
};

class Sun: public Planet
{
public:
    Sun(float radius, float density, sf::Vector2f position);
};

bool checkLineOfSight(sf::Vector2f start, sf::Vector2f end);
void clearPlanetsPath(sf::Vector2f start, sf::Vector2f& end);

#endif//PLANET_H
