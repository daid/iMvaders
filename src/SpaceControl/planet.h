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
    float density;
    float radius;
    string name;
public:

    Planet();
    
    void setName(string name);
    void setType(string type);
    void setRadius(float radius);
    void setDensity(float density);
    
    virtual void update(float delta);
    
    string getName() const { return name; }
    float getRadius() const { return radius; }
    
    float calcOrbitVelocity(float distance) const;
    float calcOrbitTime(float distance) const;
    float sphereOfInfluence() const;
    float hillSphereRadius() const; //True orbital stability is only between 1/2 and 1/3 of the hill sphere radius.
    
    sf::Vector2f gravity(sf::Vector2f position) const;
    sf::Vector2f gravity(sf::Vector2f position, float deltaTime);
    
    virtual void renderOnRadar(sf::RenderTarget& window)
    {
#ifdef DEBUG
        if (physics == SpaceObject::Orbit)
        {
            sf::CircleShape circle(hillSphereRadius() / 2.0, 64);
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(16.0);
            circle.setFillColor(sf::Color::Transparent);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(getPosition());
            window.draw(circle);

            circle.setRadius(hillSphereRadius() / 3.0);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(getPosition());
            window.draw(circle);

            circle.setRadius(orbitDistance);
            circle.setOutlineColor(sf::Color(255,0,0,128));
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(orbitTarget->getPosition());
            window.draw(circle);

            circle.setRadius(orbitDistance + hillSphereRadius() / 2.0);
            circle.setOutlineColor(sf::Color(0,255,0,128));
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(orbitTarget->getPosition());
            window.draw(circle);
            
            circle.setRadius(orbitDistance - hillSphereRadius() / 2.0);
            circle.setOutlineColor(sf::Color(0,255,0,128));
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(orbitTarget->getPosition());
            window.draw(circle);
        }
#endif
        window.draw(sprite);
    }
    
    virtual void render3D(RenderInfo* info);
};

class Sun: public Planet
{
public:
    Sun();
    
    virtual void renderOnRadar(sf::RenderTarget& window);
    virtual void render3D(RenderInfo* info);
};

bool checkLineOfSight(sf::Vector2f start, sf::Vector2f end);
bool findPathAroundPlanets(sf::Vector2f start, sf::Vector2f& end);
void validateOrbits();

#endif//PLANET_H
