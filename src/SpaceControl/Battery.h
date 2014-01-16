#ifndef BATTERY_H
#define BATTERY_H

#include "EnergyGrid.h"

class Battery : public EnergyConsumer, public EnergyProducer, public Updatable
{
public:
    const static float maxEnergyStorage = 1000;
    const static float batteryEfficency = 0.9;  //Battery efficency is usually 90% at best.
    const static float batteryLeakagePerSecond = 0.01;
    float energyStorage;
    
    Battery(EnergyGrid* grid)
    : EnergyConsumer(grid, "Battery", 0, 0), EnergyProducer(grid, "Battery"), energyStorage(0) {}

    virtual void update(float delta)
    {
        energyConsumptionRequest = (maxEnergyStorage - energyStorage) / batteryEfficency * 10.0;
        
        energyStorage += energyConsumptionAmount * delta * batteryEfficency;
        energyStorage -= energyProductionUsed * delta;
        energyStorage *= powf(1.0 - batteryLeakagePerSecond, delta);
        energyProductionAmount = energyStorage;

        //Check for short-circuit.
        foreach(EnergyConsumer, e, links)
            if (e == this)
                energyStorage = 0;
        
        if (energyStorage < 0)
            energyStorage = 0;
        if (energyStorage > maxEnergyStorage)
            energyStorage = maxEnergyStorage;
    }
};

#endif//BATTERY_H
