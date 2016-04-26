#ifndef ENERGY_GRID_H
#define ENERGY_GRID_H

class EnergyGrid;

#include "Updatable.h"
#include "Renderable.h"
#include "gui.h"

#define ENERGY_GRID_WINDOW 1

class EnergyConsumer: public virtual PObject
{
public:
    const char* energyConsumerName;
    static constexpr float idleEnergyDisabledDrainFactor = 0.8;
    
    float energyConsumptionRequest;  //Amount of energy we want to use per second, does not garantee that we'll get it.
    float energyConsumptionAmount;   //Amount of energy we are getting per second.
    float idleEnergyConsumption;     //Amount of energy we use idle, just to keep the system online.
    float powerupTimeRequired;      //Amount of time required to power up the system, going from offline to online.
    float powerupEnergyLevel;
    bool wantOnline;
    P<GuiToggle> wantOnlineGui;
    
    EnergyConsumer(P<EnergyGrid> grid, const char* name, float idleEnergyConsumption, float powerupTimeRequired);
    
    bool isOnline();
    float onlineChargeLevel();
};

class EnergyProducer: public virtual PObject
{
public:
    float energyProductionAmount;    //Amount of energy we produce per second.
    float energyProductionUsed;      //Amount of energy we are using per second.
    const char* energyProducerName;
    PVector<EnergyConsumer> links;

    EnergyProducer(P<EnergyGrid> grid, const char* name);
    
    void toggleLink(P<EnergyConsumer> consumer);
};

class EnergyGrid : public Updatable
{
public:
    PVector<EnergyProducer> producers;
    PVector<EnergyConsumer> consumers;
    
    virtual void update(float delta);
    
    virtual void render(sf::RenderTarget& window);
};

#endif//ENERGY_GRID_H
