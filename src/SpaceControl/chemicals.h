#ifndef CHEMICALS_H
#define CHEMICALS_H

#include <map>
#include <string.h>

class ChemicalInfo
{
public:
    const char* name;
    float densityAsGas, desityAsLiquid;  //kg/m^3 = g/L
    
    ChemicalInfo(const char* name, float densityAsGas, float desityAsLiquid)
    : name(name), densityAsGas(densityAsGas), desityAsLiquid(desityAsLiquid) {}
};

enum EChemical
{
    Nitrogen,
    Oxygen,
    CarbonDioxide,
    Water,
    Hydrogen,
    MAX_CHEMICALS
};

extern ChemicalInfo chemicalInfo[MAX_CHEMICALS];

class ChemicalContainer
{
public:
    float volume;   //m^3
    float contents[MAX_CHEMICALS]; //m^3 at 1atm
    
    ChemicalContainer(float volume)
    : volume(volume)
    {
        memset(contents, 0, sizeof(contents));
    }
    
    //Returns the pressure of this container in atm.
    float pressure()
    {
        float chemicalVolume = 0; //Total volume of all the chemicals at 1atm
        for(int n=0; n<MAX_CHEMICALS; n++)
            chemicalVolume += contents[n];
        return chemicalVolume / volume;
    }
    
    //Concentration of a certain chemical in the range 0.0 to 1.0
    float concentration(EChemical c)
    {
        float chemicalVolume = 0; //Total volume of all the chemicals at 1atm
        for(int n=0; n<MAX_CHEMICALS; n++)
            chemicalVolume += contents[n];
        return contents[c] / chemicalVolume * 100.0f;
    }
    
    void vent(float percentage, ChemicalContainer* target = NULL)
    {
        for(int n=0; n<MAX_CHEMICALS; n++)
        {
            float diff = contents[n] * percentage / 100.0;
            if (target)
                target->contents[n] += diff;
            contents[n] -= diff;
        }
    }
};

#endif//CHEMICALS_H
