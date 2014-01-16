#ifndef TEMPERATURE_PART_H
#define TEMPERATURE_PART_H

#include "Updatable.h"

#define TEMPERATURE_OVERVIEW_WINDOW 2

class TemperaturePart : public virtual PObject
{
public:
    float temperature;
    float temperatureIsolation; //0-100% - How well we are isolated from our parent part. 0% means our temperatures are equal. 100% means there is no heat transfer. At 50%, 50% of the temperature difference is transfered per second.
    PVector<TemperaturePart> temperatureChilds;
    
    TemperaturePart(float isolation, P<TemperaturePart> parent);    
    void updateTemperature(float delta);
};

class TemperatureRoot : public Updatable, public TemperaturePart
{
public:
    TemperatureRoot()
    : TemperaturePart(100, NULL)
    {
    }

    virtual void update(float delta);
};

#endif//TEMPERATURE_PART_H
