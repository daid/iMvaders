#ifndef RADIATOR_H
#define RADIATOR_H

#include <math.h>

#include "TemperaturePart.h"
#include "Updatable.h"

/* Thermal radiador. Radiates heat away from this part. Needs to be placed as child of the part you want to cool. */
class Radiator : public Updatable, public TemperaturePart
{
    const static float externalTemperature = -10;
public:
    Radiator(P<TemperaturePart> temperatureParent)
    : TemperaturePart(70, temperatureParent)
    {
    }
    
    virtual void update(float delta)
    {
        temperature = ((temperature - externalTemperature) * powf(0.93, delta)) + externalTemperature;//Radiate 7% heat away per second.
    }
};

#endif//RADIATOR_H
