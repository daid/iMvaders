#ifndef SOLARPANEL_H
#define SOLARPANEL_H

#include "EnergyGrid.h"
#include "TemperaturePart.h"
#include "SpaceObject.h"

class SolarPanel : public EnergyProducer, public Updatable, public TemperaturePart
{
    const static float powerFactor = 2000.0;
    const static float maxPower = 100.0;
    const static float temperaturePerPower = 0.0001;
    const static float externalTemperature = -20;
public:
    bool expand;
    P<SpaceObject> owner;
    
    SolarPanel(P<EnergyGrid> grid, P<TemperaturePart> temperatureParent, P<SpaceObject> owner)
    : EnergyProducer(grid, "SolarPanel"), TemperaturePart(95, temperatureParent), owner(owner)
    {
        expand = true;
    }
    
    virtual void update(float delta)
    {
        if (!expand)
            return;
        
        energyProductionAmount = 0.0;
        foreach(Sun, sun, sunList)
        {
            if (checkLineOfSight(sun->getPosition(), owner->getPosition()))
                continue;
            float powerAmount = (powf(sun->getRadius(), 2.0) / powf(sf::length(sun->getPosition() - owner->getPosition()), 2));
            powerAmount = powerAmount * powerFactor;
            temperature += powerAmount * temperaturePerPower;
            if (powerAmount > maxPower)
                powerAmount = maxPower;
            energyProductionAmount += powerAmount;
        }
        
        //Solar panels also radiate a bit of heat away.
        temperature = ((temperature - externalTemperature) * powf(0.97, delta)) + externalTemperature;
    }
};

#endif//GENERATOR_H
