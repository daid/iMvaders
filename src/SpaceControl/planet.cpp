#include "planet.h"
#include "textureManager.h"
#include "vectorUtils.h"

PVector<Planet> planetList;
PVector<Sun> sunList;

Planet::Planet(std::string name, int type, float radius, float density, sf::Vector2f position)
: radius(radius), name(name)
{
    planetList.push_back(this);
    alwaysVisible = true;
    mass = (radius * radius * radius) * density;
    
    switch(type)
    {
    default:
        textureManager.setTexture(sprite, "Planet1");
        break;
    case 2:
        textureManager.setTexture(sprite, "Planet2");
        break;
    }
    sprite.setScale(radius/sprite.getTextureRect().width*2, radius/sprite.getTextureRect().height*2);
    setPosition(position);
    physics = Fixed;
}

void Planet::update(float delta)
{
    SpaceObject::update(delta);
    
    foreach(SpaceObject, o, spaceObjectList)
    {
        if (o->physics == Newtonian)
        {
            float r = sf::length(o->getPosition() - getPosition());
            if (r < getRadius())
            {
                o->setPosition(getPosition() + (o->getPosition() - getPosition()) / r * (getRadius()+1));
                o->velocity *= powf(0.2, delta);
            }
            o->velocity += gravity(o->getPosition()) * delta;
        }
    }
}

sf::Vector2f Planet::gravity(sf::Vector2f position) const
{
    float r = sf::length(getPosition() - position);
    if (r < getRadius()) return sf::Vector2f(0, 0);
    return ((getPosition() - position) / r) * (mass * G) / powf(r, 2.0);
}

sf::Vector2f Planet::gravity(sf::Vector2f position, float deltaTime)
{
    sf::Vector2f p = predictPositionAtDelta(deltaTime);
    float r = sf::length(p - position);
    if (r < getRadius()) return sf::Vector2f(0, 0);
    return ((p - position) / r) * (mass * G) / powf(r, 2.0);
}

float Planet::calcOrbitVelocity(float distance) const
{
    return sqrtf((mass*G)/distance);
}
float Planet::calcOrbitTime(float distance) const
{
    return 2*M_PI*sqrtf(distance*distance*distance/(mass*G));
}

Sun::Sun(std::string name, float radius, float density, sf::Vector2f position)
: Planet(name, -1, radius, density, position)
{
    //Make the sun sprite bigger, as the outer radius of the sun is softer then of planets.
    textureManager.setTexture(sprite, "Sun");
    sprite.setScale(radius/sprite.getTextureRect().width*2, radius/sprite.getTextureRect().height*2);
    sprite.setScale(sprite.getScale() * 1.5f);
    
    sunList.push_back(this);
}

bool checkLineOfSight(sf::Vector2f start, sf::Vector2f end)
{
    if ((start - end) < 0.001f)
    {
        foreach(Planet, p, planetList)
            if (p->getPosition() - start < p->getRadius() * 0.9f)
                return false;
        return true;
    }
    foreach(Planet, p, planetList)
    {
        float d = sf::length(start - end);
        float f = sf::dot(end - start, p->getPosition() - start) / d;
        if (f < 0) f = 0;
        if (f > d) f = d;
        sf::Vector2f q = start + sf::normalize(end - start) * f;
        if ((q - p->getPosition()) < p->getRadius() * 0.9f)
            return false;
    }
    return true;
}

void clearPlanetsPath(sf::Vector2f start, sf::Vector2f& end)
{
    if ((start - end) < 0.001f)
        return;

    Planet* firstAvoid = NULL;
    float startEndLength = sf::length(start - end);
    float firstAvoidF = startEndLength;
    sf::Vector2f firstAvoidQ;
    foreach(Planet, p, planetList)
    {
        float f = sf::dot(end - start, p->getPosition() - start) / startEndLength;
        if (f < 0) continue;
        if (f > startEndLength) continue;
        sf::Vector2f q = start + (end - start) / startEndLength * f;
        if ((q - p->getPosition()) < p->getRadius() * 2.0f)
        {
            if (firstAvoid == NULL || f < firstAvoidF)
            {
                firstAvoid = *p;
                firstAvoidF = f;
                firstAvoidQ = q;
            }
        }
    }
    if (firstAvoid)
    {
        end = firstAvoid->getPosition() + sf::normalize(firstAvoidQ - firstAvoid->getPosition()) * firstAvoid->getRadius() * 2.0f;
    }
    return;
}
