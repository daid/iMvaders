#ifndef CREWCAPSULE_H
#define CREWCAPSULE_H

#include "TemperaturePart.h"
#include "chemicals.h"
#include "Updatable.h"

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
    constexpr static float oxygenUsePerPersonPerDay = 16.0; //0.8 is normal for humans under a bit of stress. m^3
    constexpr static float waterProductionPerPersonPerDay = 0.1152; //m^3
public:
    static constexpr float minO2concentration = 17.5;
    static constexpr float warningO2concentration = 18.5;
    static constexpr float warningCO2concentration = 5;
    static constexpr float maxCO2concentration = 7;
    
    static constexpr float minPressure = 0.356;
    static constexpr float warningPressure = 0.5;

    static constexpr float maxTemperature = 60;
    static constexpr float warningMaxTemperature = 35;

    static constexpr float minTemperature = 0;
    static constexpr float warningMinTemperature = 10;
    
    static constexpr float ventPercentagePerSecond = 10.0;
    
    int occupants;
    
    float o2concentration;
    float co2concentration;
    float nConcentration;
    float internalPressure;
    
    bool ventCapsule;

    CrewCapsule(float volume, int occupants, P<TemperaturePart> temperatureParent)
    : ChemicalContainer(volume), TemperaturePart(90, temperatureParent), occupants(occupants)
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
