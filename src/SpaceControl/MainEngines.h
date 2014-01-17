#ifndef MAIN_ENGINES_H
#define MAIN_ENGINES_H

class MainEngines: public EnergyConsumer, public Updatable, public TemperaturePart
{
    const static float energyRequirement = 200.0;
    const static float impulse = 300.0;
    const static float temperaturePerEnergy = 0.05;
    P<SpaceObject> owner;
public:
    float trustRequest;

    MainEngines(EnergyGrid* grid, P<SpaceObject> owner, P<TemperaturePart> temperatureParent)
    : EnergyConsumer(grid, "MainEngines", 20, 15), TemperaturePart(80, temperatureParent), owner(owner)
    {
        trustRequest = 0.0;
    }
    
    virtual void update(float delta)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            trustRequest = 1.0;
        
        trustRequest = std::max(std::min(trustRequest, 1.0f), 0.0f);
        
        energyConsumptionRequest = trustRequest * energyRequirement;

        float e = energyConsumptionAmount;
        if (e < trustRequest * energyRequirement)
        {
            trustRequest *= e / (trustRequest * energyRequirement);
        }
        if (trustRequest != 0.0)
            owner->velocity += sf::vector2FromAngle(owner->getRotation()) * impulse * delta * trustRequest;
        trustRequest = 0.0;

        temperature += energyConsumptionAmount * delta * temperaturePerEnergy;
    }
};

#endif//MAIN_ENGINES_H
