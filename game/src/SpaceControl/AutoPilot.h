#ifndef AUTOPILOT_H
#define AUTOPILOT_H

class AutoPilot : public Updatable
{
    P<SpaceObject> owner;
    P<ReactionTrusters> trusters;
public:
    P<Planet> targetPlanet;
    sf::Vector2f targetVelocity;
    float targetDistance;
    bool keepOrbit;

    AutoPilot(P<SpaceObject> owner, P<ReactionTrusters> trusters)
    : owner(owner), trusters(trusters)
    {
        keepOrbit = false;
    }

    virtual void update(float delta)
    {
        if (!keepOrbit)
        {
            foreach(Planet, planet, planetList)
            {
                if (!targetPlanet || sf::length(planet->gravity(owner->getPosition())) > sf::length(targetPlanet->gravity(owner->getPosition())))
                    targetPlanet = planet;
            }
            if (targetPlanet)
                targetDistance = sf::length(targetPlanet->getPosition() - owner->getPosition());
        }
        if (targetPlanet)
        {
            float distance = targetDistance - sf::length(targetPlanet->getPosition() - owner->getPosition());

            sf::Vector2f linearMovementRequest;
            float requiredVelocity = targetPlanet->calcOrbitVelocity(targetDistance);
            float requiredAngle = sf::vector2ToAngle(targetPlanet->getPosition() - owner->getPosition()) - 90;

            float f = distance / (targetDistance * 0.1);
            if (fabs(f) > 1.0f)
                f = f / fabs(f);
            requiredVelocity *= 1.0 + f * 0.3f;

            sf::Vector2f targetVelocityA = targetPlanet->velocity * float(1.0f - fabs(f)) + sf::vector2FromAngle(requiredAngle) * requiredVelocity;
            sf::Vector2f targetVelocityB = targetPlanet->velocity * float(1.0f - fabs(f)) + sf::vector2FromAngle(requiredAngle + 180) * requiredVelocity;
            sf::Vector2f velocityDifference;
            if ((targetVelocityA - owner->velocity) < (targetVelocityB - owner->velocity))
                velocityDifference = targetVelocityA - owner->velocity;
            else
                velocityDifference = targetVelocityB - owner->velocity;
            targetVelocity = owner->velocity + velocityDifference;

            linearMovementRequest = velocityDifference / 100.0f;

            if (keepOrbit)
            {
                trusters->linearMovementRequest.x = linearMovementRequest.x;
                trusters->linearMovementRequest.y = linearMovementRequest.y;
            }
        }
    }
};

#endif//AUTOPILOT_H
