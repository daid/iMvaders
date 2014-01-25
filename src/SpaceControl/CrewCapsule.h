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
    const static float oxygenUsePerPersonPerDay = 16.0; //0.8 is normal for humans under a bit of stress. m^3
    const static float waterProductionPerPersonPerDay = 0.1152; //m^3
public:
    static const float minO2concentration = 17.5;
    static const float maxCO2concentration = 7;
    
    static const float minPressure = 0.356;
    static const float warningPressure = 0.5;

    static const float maxTemperature = 60;
    static const float warningTemperature = 35;
    
    static const float ventPercentagePerSecond = 10.0;
    
    int occupants;
    
    float o2concentration;
    float co2concentration;
    float nConcentration;
    float internalPressure;
    
    bool ventCapsule;

    CrewCapsule(float volume, int occupants, P<TemperaturePart> temperatureParent)
    : ChemicalContainer(volume), TemperaturePart(20, temperatureParent), occupants(occupants)
    {
        //Make the default composition 20.95% oxygen and the rest nitrogen.
        contents[Oxygen] = volume * 0.2095;
        contents[Nitrogen] = volume - contents[Oxygen];
        
        ventCapsule = false;
    }
    
    virtual void update(float delta)
    {
        float oxygenUse = occupants * oxygenUsePerPersonPerDay / 24 / 60 / 60 * delta;
        contents[Oxygen] -= oxygenUse;
        contents[CarbonDioxide] += oxygenUse;
        contents[Water] += occupants * waterProductionPerPersonPerDay / 24 / 60 / 60 * delta;
        
        o2concentration = concentration(Oxygen);
        co2concentration = concentration(CarbonDioxide);
        nConcentration = concentration(Nitrogen);
        internalPressure = pressure();
        
        if (ventCapsule)
        {
            vent(ventPercentagePerSecond * internalPressure * delta);
        }
    }
};

#endif//CREWCAPSULE_H
