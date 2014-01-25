#include "windowManager.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <string>
#include <algorithm>

#include "engine.h"
#include "input.h"
#include "planet.h"
#include "httpServer.h"
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
#include "MainEngines.h"

class PlayerVessel: public SpaceObject, public Renderable
{
public:
    CrewCapsule* capsule;
    
    EnergyGrid* grid;
    TemperatureRoot* temperatureRoot;
    Radiator* mainRadiators[2];
    Generator* generator;
    Radiator* generatorRadiators[2];
    Battery* battery;
    RadarDisplay* radar;
    ReactionTrusters* reactionTrusters;
    MainEngines* engines;

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
        battery = new Battery(grid);
        radar = new RadarDisplay(grid, this);
        reactionTrusters = new ReactionTrusters(grid, this, temperatureRoot);
        engines = new MainEngines(grid, this, temperatureRoot);
        
        generator->links.push_back(battery);
        battery->links.push_back(radar);
        battery->links.push_back(reactionTrusters);
        battery->links.push_back(engines);
        
        (new GuiSlider(radar, radar->radarDistance, 512, 4096, RADAR_WINDOW, sf::FloatRect(120, 5, 50, 4)))->setCaption("Radar");
        (new GuiSlider(radar, radar->viewDistance, 512, 4096, RADAR_WINDOW, sf::FloatRect(120, 15, 50, 4)))->setCaption("View distance");
        
        (new GuiGauge(battery, battery->energyStorage, 0, battery->maxEnergyStorage, ENERGY_GRID_WINDOW, sf::FloatRect(20, 25, 50, 2)))->setCaption("Charge");
        (new GuiSlider(generator, generator->powerLevel, 0, generator->maxPowerLevel, ENERGY_GRID_WINDOW, sf::FloatRect(20, 3, 50, 4)))->setCaption("Power");
        
        (new GuiGauge(temperatureRoot, capsule->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 50, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Hull");
        (new GuiGauge(mainRadiators[0], mainRadiators[0]->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 50, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 1");
        (new GuiGauge(mainRadiators[1], mainRadiators[1]->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 60, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 2");
        (new GuiGauge(generator, generator->temperature, 0, generator->criticalTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 70, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Generator");
        (new GuiGauge(generatorRadiators[0], generatorRadiators[0]->temperature, 0, generator->criticalTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 70, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 1");
        (new GuiGauge(generatorRadiators[1], generatorRadiators[1]->temperature, 0, generator->criticalTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 80, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 2");
        (new GuiGauge(capsule, capsule->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 90, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Cabin");
        (new GuiGauge(reactionTrusters, reactionTrusters->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 100, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Reaction trusters");
        (new GuiGauge(engines, engines->temperature, 0, capsule->maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 110, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Main Engines");

        (new GuiGauge(capsule, capsule->o2concentration, capsule->minO2concentration, 22.5, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 110, 50, 4)))->setColor(sf::Color(128,255,192))->setCaption("Oxygen level");
        (new GuiGauge(capsule, capsule->co2concentration, 0, capsule->maxCO2concentration, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 120, 50, 4)))->setColor(sf::Color(128,128,128))->setCaption("CarbonDioxide level");
        (new GuiGauge(capsule, capsule->nConcentration, 0, 100, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 130, 50, 4)))->setColor(sf::Color(255,255,128))->setCaption("Nitrogen level");
        (new GuiGauge(capsule, capsule->internalPressure, 0, 1.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 140, 50, 4)))->setColor(sf::Color(128,255,128))->setCaption("Cabin Pressure");
        (new GuiButton(capsule, capsule->ventCapsule, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 150, 50, 4)))->setColor(sf::Color(255,64,64))->setCaption("Open Vents");
    }
    
    virtual void update(float delta)
    {
        SpaceObject::update(delta);
        printf("Battery: %f %f %f\n", battery->energyStorage, battery->energyProductionUsed, battery->energyConsumptionAmount);
        printf("Temperature: %f %f %f\n", temperatureRoot->temperature, generator->temperature, generatorRadiators[0]->temperature);
    }
    
    virtual void preRender(sf::RenderTarget& window) {}
    virtual void render(sf::RenderTarget& window)
    {
        radar->render(window);
        renderComponentsOfWindow(window, RADAR_WINDOW);
        
        renderComponentsOfWindow(window, ENERGY_GRID_WINDOW);
        grid->render(window);

        renderComponentsOfWindow(window, TEMPERATURE_OVERVIEW_WINDOW);
        
        if (capsule->temperature > capsule->warningTemperature)
        {
            sf::RectangleShape fullScreenRed(sf::Vector2f(320, 240));
            float alpha = 200;
            if (temperatureRoot->temperature < capsule->maxTemperature)
                alpha = (temperatureRoot->temperature - capsule->warningTemperature) / (capsule->maxTemperature - capsule->warningTemperature) * 200;
            fullScreenRed.setFillColor(sf::Color(255, 0, 0, alpha));
            window.draw(fullScreenRed);
        }

        if (capsule->internalPressure < capsule->warningPressure)
        {
            sf::RectangleShape fullScreenBlack(sf::Vector2f(320, 240));
            float alpha = 255;
            if (capsule->internalPressure > capsule->minPressure)
                alpha = (capsule->warningPressure - capsule->internalPressure) / (capsule->warningPressure - capsule->minPressure) * 255;
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
    Sun* sun = new Sun(512, 2000000000, sf::Vector2f(-700, 0));
    Planet* p = new Planet(1, 256, 2000000000, sf::Vector2f(2560, 256));
    p->setOrbit(sun, 2500, 90);
    Planet* p2 = new Planet(2, 32, 2000000000, sf::Vector2f(128, -300));
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
    
    engine->runMainLoop();
    
    delete engine;
    return 0;
}
