#ifndef PRESSURE_VALVE_H
#define PRESSURE_VALVE_H

class PressureValve : public Updatable, public EnergyConsumer
{
    const static float switchEnergy = 50;
public:
    StorageTankLinks sourceTanks;
    ChemicalContainer* target;
    float targetPressure;
    float energyCharge;
    bool reqActive, active;
    
    PressureValve(P<EnergyGrid> grid, ChemicalContainer* target, float targetPressure)
    : EnergyConsumer(grid, "PressureValve", 0, 0), target(target), targetPressure(targetPressure)
    {
        reqActive = false;
        active = reqActive;
        energyCharge = 0;
    }

    virtual void update(float delta)
    {
        if (active != reqActive)
        {
            energyConsumptionRequest = switchEnergy;
            energyCharge += energyConsumptionAmount * delta;
            
            if (energyCharge >= switchEnergy)
            {
                active = reqActive;
                energyCharge = 0;
            }
        }else{
            energyConsumptionRequest = 0;
        }
        if (!active)
            return;
        if (target->pressure() < targetPressure)
        {
            foreach(StorageTank, tank, sourceTanks.tanks)
            {
                if (tank->pressure() > target->pressure())
                    tank->vent((targetPressure - target->pressure()) * delta, target);
            }
        }
    }
};

#endif//PRESSURE_VALVE_H
