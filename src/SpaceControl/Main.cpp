#include "windowManager.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <string>
#include <algorithm>

#include "Player.h"
#include "scriptInterface.h"
#include "stringUtils.h"
#include "engine.h"
#include "input.h"
#include "planet.h"
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

        findPathAroundPlanets(getPosition(), targetPosition);
        findPathAroundPlanets(getPosition(), targetPosition);

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
    /*
    Sun* sun = new Sun();
    sun->setRadius(768);
    sun->setPosition(sf::Vector2f(-700, 0));
    Planet* p = new Planet();
    p->setName("Jamarkley IV");
    p->setRadius(350);
    p->setPosition(sf::Vector2f(2560, 256));
    p->setOrbit(sun, 3500, 90);
    Planet* p2 = new Planet();
    p2->setName("Exskoth I");
    p2->setType("Planet2");
    p2->setRadius(32);
    p2->setPosition(sf::Vector2f(128, -300));
    p2->setOrbit(p, 500, 0);

    SpaceObject* obj = new SpaceObject();
    obj->setOrbit(sun, 1200, 0);

    SpaceObject* obj2 = new SpaceObject();
    obj2->setOrbit(p, 400, 180);

    for(unsigned int n=0; n<100; n++)
    {
        AIVessel* ai = new AIVessel();
        ai->setPosition(sf::Vector2f(200 + n * 100, 600));
        ai->target = obj;
        ai->target2 = obj2;
    }
    */
    ScriptObject* so = new ScriptObject("Resources/Systems/513-920.lua");

    //player->velocity = sf::Vector2f(0, sun->calcOrbitVelocity(700));
    player->setPosition(sf::Vector2f(100, 100));

    //(new Planet("Limporyen II", 2, 512, 2000000000, sf::Vector2f(0, 0)))->setOrbit(sun, 6000, 0);
    //(new Planet("Limporyen III", 1, 512, 2000000000, sf::Vector2f(0, 0)))->setOrbit(sun, 6000, 180);
    //(new Planet("Levi-Montalcini VII", 2, 128, 2000000000, sf::Vector2f(0, 0)))->setOrbit(sun, 10000, 12489);
    //(new Planet("Mcdermott I", 2, 1024, 2000000000, sf::Vector2f(0, 0)))->setOrbit(sun, 25000, 1298);
    
    NebulaInfo info;
    for(unsigned int n=0; n<10; n++)
    {
        info.vector = sf::Vector3f(random(-1, 1), random(-1, 1), random(-1, 1));
        info.textureName = "Nebula1";
        nebulaInfo.push_back(info);
        info.vector = sf::Vector3f(random(-1, 1), random(-1, 1), random(-1, 1));
        info.textureName = "Nebula2";
        nebulaInfo.push_back(info);
        info.vector = sf::Vector3f(random(-1, 1), random(-1, 1), random(-1, 1));
        info.textureName = "Nebula3";
        nebulaInfo.push_back(info);
    }

    validateOrbits();
    engine->runMainLoop();

    delete engine;
    return 0;
}
