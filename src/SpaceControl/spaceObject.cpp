#include "spaceObject.h"
#include "textureManager.h"
#include "vectorUtils.h"
#include "planet.h"
#include "engine.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(SpaceObject)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setOrbit);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setPosition);
}

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
            if (orbitPrograde)
            {
                orbitAngle += delta / timeForOrbit * 360.0f;
                setRotation(orbitAngle + 90);
                velocity = sf::vector2FromAngle(orbitAngle + 90) * orbitTarget->calcOrbitVelocity(orbitDistance) + orbitTarget->velocity;
            }else{
                orbitAngle -= delta / timeForOrbit * 360.0f;
                setRotation(orbitAngle - 90);
                velocity = sf::vector2FromAngle(orbitAngle - 90) * orbitTarget->calcOrbitVelocity(orbitDistance) + orbitTarget->velocity;
            }
            setPosition(orbitTarget->getPosition() + sf::vector2FromAngle(orbitAngle) * orbitDistance);
        }
        break;
    }
}

void SpaceObject::setOrbit(Planet* target, float distance, float angle)
{
    physics = Orbit;
    orbitTarget = target;
    orbitDistance = abs(distance);
    orbitAngle = angle;
    orbitPrograde = distance > 0;
    SpaceObject::update(engine->getElapsedTime());//Call the update to set the position right now.
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
        float angle;
        if (orbitPrograde)
            angle = orbitAngle + delta / timeForOrbit * 360.0f;
        else
            angle = orbitAngle - delta / timeForOrbit * 360.0f;
        return orbitTarget->predictPositionAtDelta(delta) + sf::vector2FromAngle(angle) * orbitDistance;
        }
    }
    return getPosition();
}
