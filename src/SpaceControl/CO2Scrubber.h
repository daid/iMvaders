#ifndef CO2SCRUBBER_H
#define CO2SCRUBBER_H

#include "Updatable.h"
#include "EnergyGrid.h"
#include "TemperaturePart.h"

/*
 * The CO2Scrubber works as the regenerative carbon dioxide removal system (RCRS) on the space shuttle.
 * The RCRS stores CO2 from the air mixture and later vents it into space. Can be simulated by just removing CO2 from the air.
 * For gameplay reasons this uses a large amount of power when active.
 */
class CO2Scrubber: public Updatable, public EnergyConsumer, public TemperaturePart
{
public:
    constexpr static float carbonDioxideCrubbingPerSecond = 0.00001;
    constexpr static float energyRequirementWhileScrubbing = 100;
    constexpr static float temperatureIncreaseWhileScrubbing = 1;

    ChemicalContainer* container;
    bool active;

    CO2Scrubber(P<EnergyGrid> grid, ChemicalContainer* container, P<TemperaturePart> temperatureParent)
    : EnergyConsumer(grid, "CO2 Scrubber", 10, 5), TemperaturePart(40, temperatureParent), container(container)
    {
        active = false;
    }
    
    virtual void update(float delta)
    {
        if (active)
            energyConsumptionRequest = energyRequirementWhileScrubbing;
        else
            energyConsumptionRequest = 0;
        
        float change = carbonDioxideCrubbingPerSecond * energyConsumptionAmount / energyRequirementWhileScrubbing;
        if (change > container->contents[CarbonDioxide])
            change = container->contents[CarbonDioxide];
        container->contents[CarbonDioxide] -= change;
        
        temperature += temperatureIncreaseWhileScrubbing * energyConsumptionAmount / energyRequirementWhileScrubbing * delta;
    }
};

#endif//CO2SCRUBBER_H
