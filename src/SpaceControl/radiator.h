#ifndef RADIATOR_H
#define RADIATOR_H

#include "TemperaturePart.h"
#include "Updatable.h"

/* Thermal radiador. Radiates heat away from this part. Needs to be placed as child of the part you want to cool. */
class Radiator : public Updatable, public TemperaturePart
{
public:
    Radiator(P<TemperaturePart> temperatureParent)
    : TemperaturePart(50, temperatureParent)
    {
    }
    
    virtual void update(float delta)
    {
        temperature *= powf(0.93, delta);//Radiate 7% heat away per second.
    }
};

#endif//RADIATOR_H
