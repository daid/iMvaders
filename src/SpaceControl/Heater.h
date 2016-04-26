#ifndef HEATER_H
#define HEATER_H

class Heater : public Updatable, public EnergyConsumer
{
    constexpr static float energyRequirement = 100;
    P<TemperaturePart> target;
    float targetTemperature;
public:
    Heater(P<EnergyGrid> grid, P<TemperaturePart> target, float targetTemperature)
    : EnergyConsumer(grid, "Heater", 0, 0)
    {
        this->target = target;
        this->targetTemperature = targetTemperature;
    }
    
    virtual void update(float delta)
    {
        if (target->temperature < targetTemperature - 1.0)
            energyConsumptionRequest = energyRequirement;
        else if (target->temperature < targetTemperature)
            energyConsumptionRequest = (targetTemperature - target->temperature) * energyRequirement;
        else
            energyConsumptionRequest = 0;
        target->temperature += 0.01 * energyConsumptionAmount * delta;
    }
};

#endif//HEATER_H
