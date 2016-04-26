#ifndef MAIN_ENGINES_H
#define MAIN_ENGINES_H

#include "TemperaturePart.h"
#include "EnergyGrid.h"
#include "StorageTank.h"

class MainEngines: public EnergyConsumer, public Updatable, public TemperaturePart
{
    constexpr static float energyRequirement = 200.0;
    constexpr static float impulse = 300.0;
    constexpr static float temperaturePerTrust = 10.0;
    P<SpaceObject> owner;
public:
    float trustRequest;
    StorageTankLinks tanks;

    MainEngines(P<EnergyGrid> grid, P<SpaceObject> owner, P<TemperaturePart> temperatureParent)
    : EnergyConsumer(grid, "MainEngines", 20, 15), TemperaturePart(90, temperatureParent), owner(owner)
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
        {
            float totalHydrogen = tanks.total(Hydrogen);
            float totalOxygen = tanks.total(Oxygen);
            if (totalHydrogen < trustRequest * delta)
                trustRequest = totalHydrogen / delta;
            if (totalOxygen < trustRequest * delta)
                trustRequest = totalOxygen / delta;

            owner->velocity += sf::vector2FromAngle(owner->getRotation()) * impulse * delta * trustRequest;
            temperature += trustRequest * delta * temperaturePerTrust;

            tanks.drain(trustRequest * delta, Hydrogen);
            tanks.drain(trustRequest * delta, Oxygen);
        }
        trustRequest = 0.0;
    }
};

#endif//MAIN_ENGINES_H
