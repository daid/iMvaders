#ifndef REACTION_TRUSTERS_H
#define REACTION_TRUSTERS_H

class ReactionTrusters: public EnergyConsumer, public Updatable, public TemperaturePart
{
    constexpr static float energyRequirementLinear = 80.0;
    constexpr static float energyRequirementAngular = 80.0;
    constexpr static float impulseLinear = 50.0;
    constexpr static float impulseAngular = 300.0;
    constexpr static float temperaturePerEnergy = 0.02;
    P<SpaceObject> owner;
public:
    sf::Vector2f linearMovementRequest;
    float angularMovementRequest;

    ReactionTrusters(P<EnergyGrid> grid, P<SpaceObject> owner, P<TemperaturePart> temperatureParent)
    : EnergyConsumer(grid, "ReactionTrusters", 5, 10), TemperaturePart(90, temperatureParent), owner(owner), angularMovementRequest(0)
    {
    }
    
    virtual void update(float delta)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            angularMovementRequest =-1.0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            angularMovementRequest = 1.0;
        if (angularMovementRequest == 0.0)
            angularMovementRequest = -owner->angularVelocity / 50.0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            linearMovementRequest += sf::vector2FromAngle(owner->getRotation());
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            linearMovementRequest -= sf::vector2FromAngle(owner->getRotation());
        
        angularMovementRequest = std::max(std::min(angularMovementRequest, 1.0f), -1.0f);
        linearMovementRequest.x = std::max(std::min(linearMovementRequest.x, 1.0f), -1.0f);
        linearMovementRequest.y = std::max(std::min(linearMovementRequest.y, 1.0f), -1.0f);
        
        energyConsumptionRequest = abs(angularMovementRequest) * energyRequirementAngular;
        energyConsumptionRequest += sf::length(linearMovementRequest) * energyRequirementLinear;

        float e = energyConsumptionAmount;
        if ((e / energyRequirementAngular) < abs(angularMovementRequest))
        {
            angularMovementRequest = angularMovementRequest / abs(angularMovementRequest) * (e / energyRequirementAngular);
            e = 0;
        }else{
            e -= abs(angularMovementRequest) * energyRequirementAngular;
        }
        owner->angularVelocity += angularMovementRequest * impulseAngular * delta;
        angularMovementRequest = 0.0;

        if (e < sf::length(linearMovementRequest) * energyRequirementLinear)
        {
            linearMovementRequest *= e / (sf::length(linearMovementRequest) * energyRequirementLinear);
        }
        
        owner->velocity += impulseLinear * delta * linearMovementRequest;
        linearMovementRequest = sf::Vector2f(0, 0);
        
        temperature += energyConsumptionAmount * delta * temperaturePerEnergy;
    }
};

#endif//REACTION_TRUSTERS_H
