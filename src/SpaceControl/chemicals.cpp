#include "chemicals.h"

ChemicalInfo* nitrogen = new ChemicalInfo("N", 1.251, 807);          //
ChemicalInfo* oxygen = new ChemicalInfo("O2", 1.429, 1141);           //Normal human consumption: 0.6m^3 per day, needs minimal 17.5% O2 in the air mix. Around 0.5atm you get high-altitude sickness. At 0.356 atm you die. More then 50% oxygen concentration causes slight irritation, 100% oxygen does not cause death but is higly annoying.
ChemicalInfo* carbonDioxide = new ChemicalInfo("C02", 1.977, 770);   //More then 7% CO2 in the air mix causes mental confusion, and finally you fall asleep = death.
ChemicalInfo* water = new ChemicalInfo("H2O", 0, 1000);
ChemicalInfo* hydrogen = new ChemicalInfo("H2", 0.08988, 70.85);
