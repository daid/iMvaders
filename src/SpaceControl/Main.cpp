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

class ReactionTrusters: public EnergyConsumer, public Updatable, public TemperaturePart
{
    const static float energyRequirementLinear = 80.0;
    const static float energyRequirementAngular = 80.0;
    const static float impulseLinear = 50.0;
    const static float impulseAngular = 300.0;
    const static float temperaturePerEnergy = 0.05;
    SpaceObject* owner;
public:
    sf::Vector2f linearMovementRequest;
    float angularMovementRequest;

    ReactionTrusters(EnergyGrid* grid, SpaceObject* owner, P<TemperaturePart> temperatureParent)
    : EnergyConsumer(grid, "ReactionTrusters", 5, 10), TemperaturePart(80, temperatureParent), owner(owner), angularMovementRequest(0)
    {
    }
    
    virtual void update(float delta)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            angularMovementRequest =-1.0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            angularMovementRequest = 1.0;
        if (angularMovementRequest == 0.0)
            angularMovementRequest = -owner->angularVelocity / 50.0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            linearMovementRequest.x = 1.0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            linearMovementRequest.x = -1.0;
        
        angularMovementRequest = std::max(std::min(angularMovementRequest, 1.0f), -1.0f);
        linearMovementRequest.x = std::max(std::min(linearMovementRequest.x, 1.0f), -1.0f);
        linearMovementRequest.y = std::max(std::min(linearMovementRequest.y, 1.0f), -1.0f);
        
        energyConsumptionRequest = abs(angularMovementRequest) * energyRequirementAngular;
        energyConsumptionRequest += sf::length(linearMovementRequest) * energyRequirementLinear;

        float e = energyConsumptionAmount;
        if ((e / energyRequirementAngular) < abs(angularMovementRequest))
        {
            angularMovementRequest = angularMovementRequest / abs(angularMovementRequest) * (e / energyRequirementAngular);
            e = 0;
        }else{
            e -= abs(angularMovementRequest) * energyRequirementAngular;
        }
        owner->angularVelocity += angularMovementRequest * impulseAngular * delta;
        angularMovementRequest = 0.0;

        if (e < sf::length(linearMovementRequest) * energyRequirementLinear)
        {
            linearMovementRequest *= e / (sf::length(linearMovementRequest) * energyRequirementLinear);
        }
        if (linearMovementRequest.x != 0.0)
            owner->velocity += sf::vector2FromAngle(owner->getRotation()) * impulseLinear * delta * linearMovementRequest.x;
        if (linearMovementRequest.y != 0.0)
            owner->velocity += sf::vector2FromAngle(owner->getRotation() + 90) * impulseLinear * delta * linearMovementRequest.y;
        linearMovementRequest = sf::Vector2f(0, 0);
        
        temperature += energyConsumptionAmount * delta * temperaturePerEnergy;
    }
};

class MainEngines: public EnergyConsumer, public Updatable
{
    const static float energyRequirement = 200.0;
    const static float impulse = 500.0;
    SpaceObject* owner;
public:
    float trustRequest;

    MainEngines(EnergyGrid* grid, SpaceObject* owner)
    : EnergyConsumer(grid, "MainEngines", 20, 15), owner(owner)
    {
        trustRequest = 0.0;
    }
    
    virtual void update(float delta)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            trustRequest = 1.0;
        
        trustRequest = std::max(std::min(trustRequest, 1.0f), 0.0f);
        
        energyConsumptionRequest = trustRequest * energyRequirement;

        float e = energyConsumptionAmount;
        if (e < trustRequest * energyRequirement)
        {
            trustRequest *= e / (trustRequest * energyRequirement);
        }
        if (trustRequest != 0.0)
            owner->velocity += sf::vector2FromAngle(owner->getRotation()) * impulse * delta * trustRequest;
        trustRequest = 0.0;
    }
};

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
        engines = new MainEngines(grid, this);
        
        generator->links.push_back(battery);
        battery->links.push_back(radar);
        battery->links.push_back(reactionTrusters);
        battery->links.push_back(engines);
        
        
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
        renderComponentsOfWindow(window, ENERGY_GRID_WINDOW);
        grid->render(window);

        renderComponentsOfWindow(window, TEMPERATURE_OVERVIEW_WINDOW);
        
        if (temperatureRoot->temperature > 35)
        {
            sf::RectangleShape fullScreenRed(sf::Vector2f(320, 240));
            float alpha = 200;
            float maxTemp = 60;
            if (temperatureRoot->temperature < maxTemp)
                alpha = (temperatureRoot->temperature - 35) / (maxTemp - 35) * 200;
            fullScreenRed.setFillColor(sf::Color(255, 0, 0, alpha));
            window.draw(fullScreenRed);
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
