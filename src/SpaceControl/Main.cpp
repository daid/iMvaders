#include "windowManager.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <string>
#include <algorithm>

#include "stringUtils.h"
#include "engine.h"
#include "input.h"
#include "planet.h"
//#include "httpServer.h"
#include "vectorUtils.h"
#include "spaceObject.h"
#include "random.h"
#include "textureManager.h"
#include "Updatable.h"
#include "Renderable.h"
#include "chemicals.h"
#include "EnergyGrid.h"
#include "Generator.h"
#include "Battery.h"
#include "radiator.h"
#include "CrewCapsule.h"
#include "textDraw.h"
#include "RadarDisplay.h"
#include "ReactionTrusters.h"
#include "StorageTank.h"
#include "MainEngines.h"
#include "CO2Scrubber.h"
#include "SolarPanel.h"

class AutoPilot : public Updatable
{
    P<SpaceObject> owner;
    P<ReactionTrusters> trusters;
public:
    P<Planet> targetPlanet;
    sf::Vector2f targetVelocity;
    float targetDistance;
    bool keepOrbit;

    AutoPilot(P<SpaceObject> owner, P<ReactionTrusters> trusters)
    : owner(owner), trusters(trusters)
    {
        keepOrbit = false;
    }

    virtual void update(float delta)
    {
        if (keepOrbit)
        {
            float distance = targetDistance - sf::length(targetPlanet->getPosition() - owner->getPosition());

            sf::Vector2f linearMovementRequest;
            float requiredVelocity = targetPlanet->calcOrbitVelocity(targetDistance);
            float requiredAngle = sf::vector2ToAngle(targetPlanet->getPosition() - owner->getPosition()) - 90;

            float f = distance / (targetDistance * 0.1);
            if (fabs(f) > 1.0f)
                f = f / fabs(f);
            requiredVelocity *= 1.0 + f * 0.3f;

            sf::Vector2f targetVelocityA = targetPlanet->velocity * float(1.0f - fabs(f)) + sf::vector2FromAngle(requiredAngle) * requiredVelocity;
            sf::Vector2f targetVelocityB = targetPlanet->velocity * float(1.0f - fabs(f)) + sf::vector2FromAngle(requiredAngle + 180) * requiredVelocity;
            sf::Vector2f velocityDifference;
            if ((targetVelocityA - owner->velocity) < (targetVelocityB - owner->velocity))
                velocityDifference = targetVelocityA - owner->velocity;
            else
                velocityDifference = targetVelocityB - owner->velocity;
            targetVelocity = owner->velocity + velocityDifference;

            linearMovementRequest = velocityDifference / 100.0f;

            trusters->linearMovementRequest.x = linearMovementRequest.x;
            trusters->linearMovementRequest.y = linearMovementRequest.y;
            //trusters->linearMovementRequest = sf::vector2FromAngle(-owner->getRotation()) * linearMovementRequest.x + sf::vector2FromAngle(-owner->getRotation() - 90) * linearMovementRequest.y;
        }else{
            foreach(Planet, planet, planetList)
            {
                if (!targetPlanet || sf::length(planet->gravity(owner->getPosition())) > sf::length(targetPlanet->gravity(owner->getPosition())))
                    targetPlanet = planet;
            }
            if (targetPlanet)
                targetDistance = sf::length(targetPlanet->getPosition() - owner->getPosition());
        }
    }
};

class PressureValve : public Updatable, public EnergyConsumer
{
public:
    StorageTankLinks sourceTanks;
    ChemicalContainer* target;
    float targetPressure;
    bool active;

    PressureValve(P<EnergyGrid> grid, ChemicalContainer* target, float targetPressure)
    : EnergyConsumer(grid, "PressureValve", 0, 0), target(target), targetPressure(targetPressure)
    {
        active = false;
    }

    virtual void update(float delta)
    {
        if (!active)
            return;
        if (target->pressure() < targetPressure)
        {
            foreach(StorageTank, tank, sourceTanks.tanks)
            {
                if (tank->pressure() > targetPressure)
                    tank->vent(20 * (targetPressure - target->pressure()) * delta, target);
            }
        }
    }
};

class PlayerVessel: public SpaceObject, public Renderable
{
public:
    CrewCapsule* capsule;

    EnergyGrid* grid;
    TemperatureRoot* temperatureRoot;
    Radiator* mainRadiators[2];
    P<Generator> generator;
    Radiator* generatorRadiators[2];
    SolarPanel* solarPanels[2];
    Battery* battery;
    RadarDisplay* radar;
    ReactionTrusters* reactionTrusters;
    MainEngines* engines;
    StorageTank* hydrogenTanks[4];
    StorageTank* oxygenTanks[4];
    CO2Scrubber* co2Scrubber;
    AutoPilot* autoPilot;
    PressureValve* o2PressureValve;

    PlayerVessel()
    {
        alwaysVisible = true;
        textureManager.setTexture(sprite, "RadarArrow");

        grid = new EnergyGrid();
        temperatureRoot = new TemperatureRoot();

        capsule = new CrewCapsule(2.0, 1, temperatureRoot);

        mainRadiators[0] = new Radiator(temperatureRoot);
        mainRadiators[1] = new Radiator(temperatureRoot);
        generator = new Generator(grid, temperatureRoot);
        generatorRadiators[0] = new Radiator(generator);
        generatorRadiators[1] = new Radiator(generator);
        solarPanels[0] = new SolarPanel(grid, temperatureRoot, this);
        solarPanels[1] = new SolarPanel(grid, temperatureRoot, this);
        battery = new Battery(grid);
        radar = new RadarDisplay(grid, this);
        reactionTrusters = new ReactionTrusters(grid, this, temperatureRoot);
        engines = new MainEngines(grid, this, temperatureRoot);
        co2Scrubber = new CO2Scrubber(grid, capsule, temperatureRoot);
        o2PressureValve = new PressureValve(grid, capsule, 1.0);
        autoPilot = new AutoPilot(this, reactionTrusters);

        for(int n=0;n<4;n++)
        {
            hydrogenTanks[n] = new StorageTank(1.0);
            hydrogenTanks[n]->contents[Hydrogen] = 10.0;
            oxygenTanks[n] = new StorageTank(1.0);
            oxygenTanks[n]->contents[Oxygen] = 10.0;

            engines->tanks.add(hydrogenTanks[n]);
            engines->tanks.add(oxygenTanks[n]);
            o2PressureValve->sourceTanks.add(oxygenTanks[n]);
        }

        generator->links.push_back(battery);
        solarPanels[0]->links.push_back(battery);
        solarPanels[1]->links.push_back(battery);
        battery->links.push_back(radar);
        battery->links.push_back(reactionTrusters);
        battery->links.push_back(engines);
        battery->links.push_back(co2Scrubber);

        (new GuiSlider(radar, radar->radarDistance, 512, 4096, RADAR_WINDOW, sf::FloatRect(120, 5, 50, 4)))->setCaption("Radar");
        (new GuiSlider(radar, radar->viewDistance, 512, 4096, RADAR_WINDOW, sf::FloatRect(120, 15, 50, 4)))->setCaption("View distance");

        (new GuiGauge(battery, battery->energyStorage, 0, battery->maxEnergyStorage, ENERGY_GRID_WINDOW, sf::FloatRect(20, 45, 50, 2)))->setCaption("Charge");
        (new GuiSlider(generator, generator->powerLevel, 0, Generator::maxPowerLevel, ENERGY_GRID_WINDOW, sf::FloatRect(20, 3, 50, 4)))->setCaption("Power");

        (new GuiToggle(co2Scrubber, co2Scrubber->active, ENERGY_GRID_WINDOW, sf::FloatRect(220, 50, 15, 4)))->setCaption("Active");
        (new GuiToggle(o2PressureValve, o2PressureValve->active, ENERGY_GRID_WINDOW, sf::FloatRect(220, 60, 15, 4)))->setCaption("Active");

        (new GuiGauge(temperatureRoot, capsule->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 50, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Hull");
        (new GuiGauge(mainRadiators[0], mainRadiators[0]->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 50, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 1");
        (new GuiGauge(mainRadiators[1], mainRadiators[1]->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 60, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 2");
        (new GuiGauge(generator, generator->temperature, 0, Generator::criticalTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 70, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Generator");
        (new GuiGauge(generatorRadiators[0], generatorRadiators[0]->temperature, 0, Generator::criticalTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 70, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 1");
        (new GuiGauge(generatorRadiators[1], generatorRadiators[1]->temperature, 0, Generator::criticalTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 80, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 2");
        (new GuiGauge(capsule, capsule->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 90, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Cabin");
        (new GuiGauge(reactionTrusters, reactionTrusters->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 100, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Reaction trusters");
        (new GuiGauge(engines, engines->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 110, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Main Engines");
        (new GuiGauge(solarPanels[0], solarPanels[0]->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 120, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Solar Panel 1");

        (new GuiGauge(capsule, capsule->o2concentration, capsule->minO2concentration, 22.5, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 110, 50, 4)))->setColor(sf::Color(128,255,192))->setCaption("Oxygen level");
        (new GuiGauge(capsule, capsule->co2concentration, 0, capsule->maxCO2concentration, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 120, 50, 4)))->setColor(sf::Color(128,128,128))->setCaption("CarbonDioxide level");
        (new GuiGauge(capsule, capsule->nConcentration, 0, 100, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 130, 50, 4)))->setColor(sf::Color(255,255,128))->setCaption("Nitrogen level");
        (new GuiGauge(capsule, capsule->internalPressure, capsule->minPressure, 1.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 140, 50, 4)))->setColor(sf::Color(128,255,128))->setCaption("Cabin Pressure");
        (new GuiButton(capsule, capsule->ventCapsule, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 150, 50, 4)))->setColor(sf::Color(255,64,64))->setCaption("Open Vents");

        (new GuiGauge(hydrogenTanks[0], hydrogenTanks[0]->contents[Hydrogen], 0, 10.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 150, 50, 4)))->setColor(sf::Color(255,255,255))->setCaption("Hydrogen");
        (new GuiGauge(hydrogenTanks[1], hydrogenTanks[1]->contents[Hydrogen], 0, 10.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 155, 50, 4)))->setColor(sf::Color(255,255,255));
        (new GuiGauge(hydrogenTanks[2], hydrogenTanks[2]->contents[Hydrogen], 0, 10.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 160, 50, 4)))->setColor(sf::Color(255,255,255));
        (new GuiGauge(hydrogenTanks[3], hydrogenTanks[3]->contents[Hydrogen], 0, 10.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 165, 50, 4)))->setColor(sf::Color(255,255,255));

        (new GuiGauge(oxygenTanks[0], oxygenTanks[0]->contents[Oxygen], 0, 10.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 175, 50, 4)))->setColor(sf::Color(128,128,255))->setCaption("Oxygen");
        (new GuiGauge(oxygenTanks[1], oxygenTanks[1]->contents[Oxygen], 0, 10.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 180, 50, 4)))->setColor(sf::Color(128,128,255));
        (new GuiGauge(oxygenTanks[2], oxygenTanks[2]->contents[Oxygen], 0, 10.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 185, 50, 4)))->setColor(sf::Color(128,128,255));
        (new GuiGauge(oxygenTanks[3], oxygenTanks[3]->contents[Oxygen], 0, 10.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 190, 50, 4)))->setColor(sf::Color(128,128,255));

        (new GuiGauge(generator, generator->contents[Uranium], 0, generator->volume, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 200, 50, 4)))->setColor(sf::Color(64,255,128))->setCaption("Uranium");

        (new GuiToggle(autoPilot, autoPilot->keepOrbit, RADAR_WINDOW, sf::FloatRect(20, 220, 50, 4)))->setColor(sf::Color(64,64,64))->setCaption("Keep Orbit");
    }

    virtual void update(float delta)
    {
        SpaceObject::update(delta);
        printf("Battery: %f %f %f\n", battery->energyStorage, battery->energyProductionUsed, battery->energyConsumptionAmount);
        //printf("Temperature: %f %f %f\n", temperatureRoot->temperature, generator->temperature, generatorRadiators[0]->temperature);
    }

    virtual void preRender(sf::RenderTarget& window) {}
    virtual void render(sf::RenderTarget& window)
    {
        radar->render(window);
        renderComponentsOfWindow(window, RADAR_WINDOW);

        renderComponentsOfWindow(window, ENERGY_GRID_WINDOW);
        grid->render(window);

        renderComponentsOfWindow(window, TEMPERATURE_OVERVIEW_WINDOW);

        if (autoPilot->targetPlanet)
        {
            drawText(window, 20, 230, 0.4, autoPilot->targetPlanet->getName() + " " + to_string(int(autoPilot->targetDistance)), align_left);

            sf::VertexArray a(sf::Lines, 2);
            a[0].position = sf::Vector2f(160, 120) + autoPilot->targetVelocity / 10.0f;
            a[1].position = sf::Vector2f(160, 120) + velocity / 10.0f;
            window.draw(a);
        }

        //Draw full screen temperature overlay when we are overheating.
        if (capsule->temperature > capsule->warningMaxTemperature)
        {
            sf::RectangleShape fullScreenRed(sf::Vector2f(320, 240));
            float alpha = 200;
            if (capsule->temperature < capsule->maxTemperature)
                alpha = (capsule->temperature - capsule->warningMaxTemperature) / (capsule->maxTemperature - capsule->warningMaxTemperature) * 200;
            fullScreenRed.setFillColor(sf::Color(255, 0, 0, alpha));
            window.draw(fullScreenRed);
        }

        //Draw full screen temperature overlay when we are freezing.
        if (capsule->temperature < capsule->warningMinTemperature)
        {
            sf::RectangleShape fullScreenBlack(sf::Vector2f(320, 240));
            float alpha = 255;
            if (capsule->temperature > capsule->minTemperature)
                alpha = (capsule->warningMinTemperature - capsule->temperature) / (capsule->warningMinTemperature - capsule->minTemperature) * 255;
            fullScreenBlack.setFillColor(sf::Color(64, 64, 255, alpha));
            window.draw(fullScreenBlack);
        }

        //Draw full screen blackout overlay when we are blacking out due to pressure loss.
        if (capsule->internalPressure < capsule->warningPressure)
        {
            sf::RectangleShape fullScreenBlack(sf::Vector2f(320, 240));
            float alpha = 255;
            if (capsule->internalPressure > capsule->minPressure)
                alpha = (capsule->warningPressure - capsule->internalPressure) / (capsule->warningPressure - capsule->minPressure) * 255;
            fullScreenBlack.setFillColor(sf::Color(0, 0, 0, alpha));
            window.draw(fullScreenBlack);
        }

        //Draw full screen blackout overlay when we are blacking out because of loss of oxygen.
        if (capsule->o2concentration < capsule->warningO2concentration)
        {
            sf::RectangleShape fullScreenBlack(sf::Vector2f(320, 240));
            float alpha = 255;
            if (capsule->o2concentration > capsule->minO2concentration)
                alpha = (capsule->warningO2concentration - capsule->o2concentration) / (capsule->warningO2concentration - capsule->minO2concentration) * 255;
            fullScreenBlack.setFillColor(sf::Color(0, 0, 0, alpha));
            window.draw(fullScreenBlack);
        }

        //Draw full screen blackout overlay when we are blacking out because of loss of oxygen.
        if (capsule->co2concentration > capsule->warningCO2concentration)
        {
            sf::RectangleShape fullScreenBlack(sf::Vector2f(320, 240));
            float alpha = 255;
            if (capsule->co2concentration < capsule->maxCO2concentration)
                alpha = (capsule->co2concentration - capsule->warningCO2concentration) / (capsule->maxCO2concentration - capsule->warningCO2concentration) * 255;
            fullScreenBlack.setFillColor(sf::Color(0, 0, 0, alpha));
            window.draw(fullScreenBlack);
        }
    }
    virtual void postRender(sf::RenderTarget& window) {}
};

class AIVessel: public SpaceObject
{
public:
    P<SpaceObject> target;
    P<SpaceObject> target2;
    sf::Vector2f targetPosition;

    AIVessel()
    {
        textureManager.setTexture(sprite, "RadarArrow");
    }

    virtual void update(float delta)
    {
        SpaceObject::update(delta);

        targetPosition = target->getPosition();

        clearPlanetsPath(getPosition(), targetPosition);
        clearPlanetsPath(getPosition(), targetPosition);

        sf::Vector2f diff = targetPosition - getPosition();
        sf::Vector2f targetVelocity = sf::normalize(diff) * 200.0f + target->velocity;
        float dist = sf::length(diff);
        float slowdownDistance = (sf::length(velocity) - sf::length(target->velocity)) / 1.0f;
        if (dist < slowdownDistance)
        {
            targetVelocity = ((targetVelocity * dist) + (target->velocity * (slowdownDistance - dist))) / slowdownDistance;
        }
        if ((target->getPosition() - getPosition()) < 10.0f && (target->velocity - velocity) < 10.0f)
        {
            P<SpaceObject> tmp = target;
            target = target2;
            target2 = tmp;
        }
        setRotation(sf::vector2ToAngle(targetVelocity - velocity));
        velocity += (targetVelocity - velocity) * delta;
    }

    virtual void renderOnRadar(sf::RenderTarget& window)
    {
        SpaceObject::renderOnRadar(window);
        /*
        sf::VertexArray a(sf::Lines, 2);
        a[0].position = getPosition();
        a[1].position = targetPosition;
        window.draw(a);
        */
    }
};

int main()
{
    new Engine();
    engine->registerObject("inputHandler", new InputHandler());

    engine->registerObject("windowManager", new WindowManager(320, 240, false));

    PlayerVessel* player = new PlayerVessel();
    Sun* sun = new Sun("Sun", 512, 2000000000, sf::Vector2f(-700, 0));
    Planet* p = new Planet("Jamarkley IV", 1, 256, 2000000000, sf::Vector2f(2560, 256));
    p->setOrbit(sun, 2500, 90);
    Planet* p2 = new Planet("Exskoth I", 2, 32, 2000000000, sf::Vector2f(128, -300));
    p2->setOrbit(p, 800, 0);

    SpaceObject* obj = new SpaceObject();
    obj->setOrbit(sun, 1200, 0);

    SpaceObject* obj2 = new SpaceObject();
    obj2->setOrbit(p2, 300, 180);

    for(unsigned int n=0; n<100; n++)
    {
        AIVessel* ai = new AIVessel();
        ai->setPosition(sf::Vector2f(200 + n * 100, 600));
        ai->target = obj;
        ai->target2 = obj2;
    }

    player->velocity = sf::Vector2f(0, sun->calcOrbitVelocity(700));

    (new Planet("Limporyen II", 2, 512, 2000000000, sf::Vector2f(0, 0)))->setOrbit(sun, 5000, 0);
    (new Planet("Limporyen III", 1, 512, 2000000000, sf::Vector2f(0, 0)))->setOrbit(sun, 5000, 180);
    (new Planet("Levi-Montalcini VII", 2, 128, 2000000000, sf::Vector2f(0, 0)))->setOrbit(sun, 10000, 12489);
    (new Planet("Mcdermott I", 2, 1024, 2000000000, sf::Vector2f(0, 0)))->setOrbit(sun, 25000, 1298);

    engine->runMainLoop();

    delete engine;
    return 0;
}
