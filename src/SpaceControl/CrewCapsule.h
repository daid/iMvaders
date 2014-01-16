#ifndef CREWCAPSULE_H
#define CREWCAPSULE_H

#include "TemperaturePart.h"

//Oxygen:
//  Normal human consumption: 0.6m^3 per day
//  Needs minimal 17.5% O2 in the air mix.
//  Around 0.5atm you get high-altitude sickness.
//  At 0.356 atm you die.
//  More then 50% oxygen concentration causes slight irritation, 100% oxygen does not cause death but is higly annoying.
//CarbonDioxide
//  More then 7% CO2 in the air mix causes mental confusion, and finally you fall asleep = death.

class CrewCapsule : public ChemicalContainer, public Updatable, public TemperaturePart
{
    const static float oxygenUsePerPersonPerDay = 0.8; //m^3 (higher then average to simulate the stress on the people)
    const static float waterProductionPerPersonPerDay = 0.1152; //m^3
public:
    static const float maxTemperature = 60;
    static const float warningTemperature = 35;
    int occupants;

    CrewCapsule(float volume, int occupants, P<TemperaturePart> temperatureParent)
    : ChemicalContainer(volume), TemperaturePart(20, temperatureParent), occupants(occupants)
    {
        //Make the default composition 20.95% oxygen and the rest nitrogen.
        contents[oxygen] = volume * 0.2095;
        contents[nitrogen] = volume - contents[oxygen];
    }
    
    virtual void update(float delta)
    {
        float oxygenUse = occupants * oxygenUsePerPersonPerDay / 24 / 60 / 60 * delta;
        contents[oxygen] -= oxygenUse;
        contents[carbonDioxide] += oxygenUse;
        contents[water] += occupants * waterProductionPerPersonPerDay / 24 / 60 / 60 * delta;
    }
};

#endif//CREWCAPSULE_H
