#ifndef CHEMICALS_H
#define CHEMICALS_H

#include <map>

class ChemicalInfo
{
public:
    const char* name;
    float densityAsGas, desityAsLiquid;  //kg/m^3 = g/L
    
    ChemicalInfo(const char* name, float densityAsGas, float desityAsLiquid)
    : name(name), densityAsGas(densityAsGas), desityAsLiquid(desityAsLiquid) {}
};

extern ChemicalInfo* nitrogen;
extern ChemicalInfo* oxygen;
extern ChemicalInfo* carbonDioxide;
extern ChemicalInfo* water;
extern ChemicalInfo* hydrogen;

class ChemicalContainer
{
public:
    typedef std::map<ChemicalInfo*, float> ChemicalMap;
    
    float volume;   //m^3
    ChemicalMap contents; //m^3 at 1atm
    
    ChemicalContainer(float volume)
    : volume(volume)
    {
        contents[nitrogen] = 0.0;
        contents[oxygen] = 0.0;
        contents[carbonDioxide] = 0.0;
        contents[water] = 0.0;
        contents[hydrogen] = 0.0;
    }
    
    //Returns the pressure of this container in atm.
    float pressure()
    {
        float chemicalVolume = 0; //Total volume of all the chemicals at 1atm
        for(ChemicalMap::iterator iter = contents.begin(); iter != contents.end(); iter++)
            chemicalVolume += iter->second;
        return volume / chemicalVolume;
    }
    
    //Concentration of a certain chemical in the range 0.0 to 1.0
    float concentration(ChemicalInfo* c)
    {
        float chemicalVolume = 0; //Total volume of all the chemicals at 1atm
        for(ChemicalMap::iterator iter = contents.begin(); iter != contents.end(); iter++)
            chemicalVolume += iter->second;
        return contents[c] / chemicalVolume;
    }
};

#endif//CHEMICALS_H
