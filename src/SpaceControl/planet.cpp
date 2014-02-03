#include "planet.h"
#include "textureManager.h"
#include "vectorUtils.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_SUBCLASS(Planet, SpaceObject)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setRadius);
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setName);
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setType);
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setDensity);
}
REGISTER_SCRIPT_SUBCLASS(Sun, Planet)
{
}

PVector<Planet> planetList;
PVector<Sun> sunList;

Planet::Planet()
: density(2000000000), radius(128), name("Unknown")
{
    planetList.push_back(this);
    alwaysVisible = true;
    
    textureManager.setTexture(sprite, "Planet1");
    physics = Fixed;

    mass = (radius * radius * radius) * density;
    sprite.setScale(radius/sprite.getTextureRect().width*2, radius/sprite.getTextureRect().height*2);
}

void Planet::setName(std::string name)
{
    this->name = name;
}
void Planet::setType(std::string type)
{
    textureManager.setTexture(sprite, type);
    sprite.setScale(radius/sprite.getTextureRect().width*2, radius/sprite.getTextureRect().height*2);
}
void Planet::setRadius(float radius)
{
    this->radius = radius;
    mass = (radius * radius * radius) * density;
    sprite.setScale(radius/sprite.getTextureRect().width*2, radius/sprite.getTextureRect().height*2);
}
void Planet::setDensity(float density)
{
    this->density = density;
    mass = (radius * radius * radius) * density;
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
float Planet::sphereOfInfluence() const
{
    if (physics != Orbit)
        return INFINITY;
    return orbitDistance * powf(mass / orbitTarget->mass, 2.0f/5.0f);
}
float Planet::hillSphereRadius() const
{
    if (physics != Orbit)
        return INFINITY;
    return orbitDistance * powf(mass / (3 * orbitTarget->mass), 1.0f/3.0f);
}

Sun::Sun()
: Planet()
{
    setName("Sun");
    //Make the sun sprite bigger, as the outer radius of the sun is softer then of planets.
    textureManager.setTexture(sprite, "Sun");
    
    sunList.push_back(this);
}

void Sun::renderOnRadar(sf::RenderTarget& window)
{
    sprite.setScale(getRadius()/sprite.getTextureRect().width*2, getRadius()/sprite.getTextureRect().height*2);
    sprite.setScale(sprite.getScale() * 1.5f);

    Planet::renderOnRadar(window);
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

bool findPathAroundPlanets(sf::Vector2f start, sf::Vector2f& end)
{
    if ((start - end) < 0.001f)
        return false;

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
        return true;
    }
    return false;
}

void validateOrbits()
{
    foreach(Planet, p, planetList)
    {
        if (p->physics != SpaceObject::Orbit)
            continue;
        if (p->getRadius() > p->hillSphereRadius() / 3.0)
            printf("%s size is bigger then the minimum orbit distance\n", p->getName().c_str());
        float r = p->orbitTarget->hillSphereRadius();
        if (p->orbitDistance < r/3.0 && r != INFINITY)
            printf("%s too close to %s (%f < %f)\n", p->getName().c_str(), p->orbitTarget->getName().c_str(), p->orbitDistance, r/3.0);
        if (p->orbitDistance > r/2.0)
            printf("%s too far from %s (%f > %f)\n", p->getName().c_str(), p->orbitTarget->getName().c_str(), p->orbitDistance, r/2.0);
        
        foreach(Planet, p2, planetList)
        {
            if (p == p2 || p2->physics != SpaceObject::Orbit || p2->orbitTarget != p->orbitTarget)
                continue;
            r = p2->hillSphereRadius() / 2.0 + p->hillSphereRadius() / 2.0;
            if (fabs(p2->orbitDistance - p->orbitDistance) < r && !(p2->orbitDistance == p->orbitDistance && p->orbitAngle != p2->orbitAngle))
                printf("%s SoI collides with SoI of %s (%f < %f)\n", p->getName().c_str(), p2->getName().c_str(), fabs(p2->orbitDistance - p->orbitDistance), r);
        }
    }
    foreach(SpaceObject, obj, spaceObjectList)
    {
        if (obj->physics != SpaceObject::Orbit)
            continue;
        P<Planet> p = obj;
        if (p)
            continue;
        
        float r = obj->orbitTarget->hillSphereRadius();
        if (obj->orbitDistance < r/3.0 && r != INFINITY)
            printf("%p too close to %s (%f < %f)\n", *obj, obj->orbitTarget->getName().c_str(), obj->orbitDistance, r/3.0);
        if (obj->orbitDistance > r/2.0)
            printf("%p too far from %s (%f > %f)\n", *obj, obj->orbitTarget->getName().c_str(), obj->orbitDistance, r/2.0);
    }
}
