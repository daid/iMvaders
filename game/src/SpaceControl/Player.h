#ifndef PLAYER_H
#define PLAYER_H

#include "spaceObject.h"
#include "Renderable.h"
#include "CrewCapsule.h"
#include "EnergyGrid.h"
#include "Radiator.h"
#include "Generator.h"
#include "SolarPanel.h"
#include "Heater.h"
#include "Battery.h"
#include "radarDisplay.h"
#include "ReactionTrusters.h"
#include "MainEngines.h"
#include "StorageTank.h"
#include "CO2Scrubber.h"
#include "AutoPilot.h"
#include "PressureValve.h"

class PlayerVessel: public SpaceObject, public Renderable
{
public:
    int currentView;

    P<CrewCapsule> capsule;

    P<EnergyGrid> grid;
    P<TemperatureRoot> temperatureRoot;
    P<Radiator> mainRadiators[2];
    P<Generator> generator;
    P<Radiator> generatorRadiators[2];
    P<SolarPanel> solarPanels[2];
    P<Heater> heater;
    P<Battery> battery;
    P<RadarDisplay> radar;
    P<ReactionTrusters> reactionTrusters;
    P<MainEngines> engines;
    P<StorageTank> hydrogenTanks[4];
    P<StorageTank> oxygenTanks[4];
    P<CO2Scrubber> co2Scrubber;
    P<AutoPilot> autoPilot;
    P<PressureValve> o2PressureValve;

    PlayerVessel();

    virtual void update(float delta);

    virtual void render(sf::RenderTarget& window);
};

#endif//PLAYER_H
