#include "spaceObject.h"
#include "textureManager.h"
#include "vectorUtils.h"
#include "planet.h"

PVector<SpaceObject> spaceObjectList;

SpaceObject::SpaceObject()
: alwaysVisible(false), angularVelocity(0)
{
    physics = Newtonian;
    
    spaceObjectList.push_back(this);
    textureManager.setTexture(sprite, "RadarBlip");
}

void SpaceObject::update(float delta)
{
    switch(physics)
    {
    case Fixed:
        velocity = sf::Vector2f(0, 0);
        break;
    case Newtonian:
        setPosition(getPosition() + velocity * delta);
        setRotation(getRotation() + angularVelocity * delta);
        break;
    case Orbit:
        if (!orbitTarget)
        {
            physics = Newtonian;
        }else{
            float timeForOrbit = orbitTarget->calcOrbitTime(orbitDistance);
            orbitAngle += delta / timeForOrbit * 360.0f;
            setPosition(orbitTarget->getPosition() + sf::vector2FromAngle(orbitAngle) * orbitDistance);
            setRotation(orbitAngle + 90);
            velocity = sf::vector2FromAngle(orbitAngle + 90) * orbitTarget->calcOrbitVelocity(orbitDistance) + orbitTarget->velocity;
        }
        break;
    }
}

void SpaceObject::setOrbit(Planet* target, float distance, float angle)
{
    physics = Orbit;
    orbitTarget = target;
    orbitDistance = distance;
    orbitAngle = angle;
    SpaceObject::update(0);//Call the update to set the position right now.
}

sf::Vector2f SpaceObject::predictPositionAtDelta(float delta)
{
    switch(physics)
    {
    case Fixed:
        return getPosition();
    case Newtonian:
        return getPosition() + velocity * delta;
    case Orbit: {
        float timeForOrbit = orbitTarget->calcOrbitTime(orbitDistance);
        float angle = orbitAngle + delta / timeForOrbit * 360.0f;
        return orbitTarget->predictPositionAtDelta(delta) + sf::vector2FromAngle(angle) * orbitDistance;
        }
    }
    return getPosition();
}
