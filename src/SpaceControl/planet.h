#ifndef PLANET_H
#define PLANET_H

#include "spaceObject.h"

class Planet;
class Sun;
extern PVector<Planet> planetList;
extern PVector<Sun> sunList;

class Planet: public SpaceObject
{
    const static float G = 0.0000000000667;
    float mass;
    float radius;
    std::string name;
public:

    Planet(std::string name, int type, float radius, float density, sf::Vector2f position);
    
    virtual void update(float delta);
    
    std::string getName() const { return name; }
    float getRadius() const { return radius; }
    
    float calcOrbitVelocity(float distance) const;
    float calcOrbitTime(float distance) const;
    
    sf::Vector2f gravity(sf::Vector2f position) const;
    sf::Vector2f gravity(sf::Vector2f position, float deltaTime);
};

class Sun: public Planet
{
public:
    Sun(std::string name, float radius, float density, sf::Vector2f position);
};

bool checkLineOfSight(sf::Vector2f start, sf::Vector2f end);
void clearPlanetsPath(sf::Vector2f start, sf::Vector2f& end);

#endif//PLANET_H
