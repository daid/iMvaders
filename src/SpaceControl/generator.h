#ifndef GENERATOR_H
#define GENERATOR_H

#include "EnergyGrid.h"
#include "TemperaturePart.h"
#include "chemicals.h"

class Generator : public EnergyProducer, public Updatable, public TemperaturePart, public ChemicalContainer
{
public:
    const static float heatPerSecond = 50;
    const static float criticalTemperature = 600;
    const static float damageTemperature = 450;
    const static float powerProduction = 200;
    const static float maxPowerLevel = 2.0;
    const static float uraniumPerEnergy = 1.0 / 1000000.0;

    float powerLevel;
    
    Generator(P<EnergyGrid> grid, P<TemperaturePart> temperatureParent)
    : EnergyProducer(grid, "Generator"), TemperaturePart(99.5, temperatureParent), ChemicalContainer(1.0), powerLevel(1.0)
    {
        temperature = 400;
        contents[Uranium] = volume;
    }
    
    virtual void update(float delta)
    {
        energyProductionAmount = powerProduction * powerLevel;
        if (energyProductionAmount * uraniumPerEnergy > contents[Uranium])
            energyProductionAmount = contents[Uranium] / uraniumPerEnergy;
        contents[Uranium] -= energyProductionAmount * uraniumPerEnergy * delta;
        if (temperature > damageTemperature)    //When we are overheating we lose effectiveness.
            energyProductionAmount -= (temperature - damageTemperature) / (criticalTemperature - damageTemperature) * powerProduction / 2.0;
        if (temperature > criticalTemperature)
        {
            //...boom!
            destroy();
        }
        temperature += powerLevel * delta * heatPerSecond;
    }
};

#endif//GENERATOR_H
