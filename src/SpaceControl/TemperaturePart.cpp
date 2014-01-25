#include <math.h>
#include "TemperaturePart.h"

TemperaturePart::TemperaturePart(float isolation, P<TemperaturePart> parent)
: temperature(20), temperatureIsolation(isolation)
{
    if (parent)
    {
        parent->temperatureChilds.push_back(this);
        temperature = parent->temperature;
    }
}

void TemperaturePart::updateTemperature(float delta)
{
    foreach(TemperaturePart, tp, temperatureChilds)
    {
        tp->updateTemperature(delta);
        
        float diff = (tp->temperature - temperature);
        float temperatureDelta = (diff * (100.0 - tp->temperatureIsolation) / 100.0) * delta;
        temperature += temperatureDelta;
        tp->temperature -= temperatureDelta;
    }
}

void TemperatureRoot::update(float delta)
{
    updateTemperature(delta);
}
