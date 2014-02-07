#include "textureManager.h"
#include "stringUtils.h"
#include "textDraw.h"
#include "Player.h"
#include "SpaceRenderer.h"

PlayerVessel::PlayerVessel()
{
    currentView = 0;
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
    co2Scrubber = new CO2Scrubber(grid, *capsule, temperatureRoot);
    o2PressureValve = new PressureValve(grid, *capsule, 1.0);
    heater = new Heater(grid, capsule, 15);
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
    battery->links.push_back(o2PressureValve);
    battery->links.push_back(heater);

    (new GuiSlider(radar, radar->viewDistance, 512, 1024 * 16, RADAR_WINDOW, sf::FloatRect(120, 5, 50, 4)))->setCaption("View distance");
    (new GuiSlider(radar, radar->radarDistance, 512, 4096, RADAR_WINDOW, sf::FloatRect(120, 15, 50, 4)))->setCaption("Radar");

    (new GuiSlider(radar, radar->directionalDistance, 0, 1024 * 16, RADAR_WINDOW, sf::FloatRect(120, 25, 50, 4)))->setCaption("Directional Range");
    (new GuiSlider(radar, radar->directionalAngle, 0, 360, RADAR_WINDOW, sf::FloatRect(120, 35, 50, 4)))->setCaption("Angle");
    (new GuiSlider(radar, radar->directionalWidth, 5, 35, RADAR_WINDOW, sf::FloatRect(120, 45, 50, 4)))->setCaption("Width");

    (new GuiGauge(battery, battery->energyStorage, 0, Battery::maxEnergyStorage, ENERGY_GRID_WINDOW, sf::FloatRect(20, 45, 50, 2)))->setCaption("Charge");
    (new GuiSlider(generator, generator->powerLevel, 0, Generator::maxPowerLevel, ENERGY_GRID_WINDOW, sf::FloatRect(20, 3, 50, 4)))->setCaption("Power");

    (new GuiToggle(co2Scrubber, co2Scrubber->active, ENERGY_GRID_WINDOW, sf::FloatRect(220, 50, 15, 4)))->setCaption("Active");
    (new GuiToggle(o2PressureValve, o2PressureValve->reqActive, ENERGY_GRID_WINDOW, sf::FloatRect(220, 60, 15, 4)))->setCaption("Active");

    (new GuiGauge(temperatureRoot, capsule->temperature, 0, CrewCapsule::maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 50, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Hull");
    (new GuiGauge(mainRadiators[0], mainRadiators[0]->temperature, 0, CrewCapsule::maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 50, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 1");
    (new GuiGauge(mainRadiators[1], mainRadiators[1]->temperature, 0, CrewCapsule::maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 60, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 2");
    (new GuiGauge(generator, generator->temperature, 0, Generator::criticalTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 70, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Generator");
    (new GuiGauge(generatorRadiators[0], generatorRadiators[0]->temperature, 0, Generator::criticalTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 70, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 1");
    (new GuiGauge(generatorRadiators[1], generatorRadiators[1]->temperature, 0, Generator::criticalTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(75, 80, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Radiator 2");
    (new GuiGauge(capsule, capsule->temperature, 0, CrewCapsule::maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 90, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Cabin");
    (new GuiGauge(reactionTrusters, reactionTrusters->temperature, 0, CrewCapsule::maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 100, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Reaction trusters");
    (new GuiGauge(engines, engines->temperature, 0, CrewCapsule::maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 110, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Main Engines");
    (new GuiGauge(solarPanels[0], solarPanels[0]->temperature, 0, CrewCapsule::maxTemperature, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(20, 120, 50, 4)))->setPostfix("C")->setColor(sf::Color::Red)->setCaption("Solar Panel 1");

    (new GuiGauge(capsule, capsule->o2concentration, CrewCapsule::minO2concentration, 22.5, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 110, 50, 4)))->setColor(sf::Color(128,255,192))->setCaption("Oxygen level");
    (new GuiGauge(capsule, capsule->co2concentration, 0, CrewCapsule::maxCO2concentration, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 120, 50, 4)))->setColor(sf::Color(128,128,128))->setCaption("CarbonDioxide level");
    (new GuiGauge(capsule, capsule->nConcentration, 0, 100, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 130, 50, 4)))->setColor(sf::Color(255,255,128))->setCaption("Nitrogen level");
    (new GuiGauge(capsule, capsule->internalPressure, CrewCapsule::minPressure, 1.0, TEMPERATURE_OVERVIEW_WINDOW, sf::FloatRect(250, 140, 50, 4)))->setColor(sf::Color(128,255,128))->setCaption("Cabin Pressure");
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

void PlayerVessel::update(float delta)
{
    SpaceObject::update(delta);
    //printf("Battery: %f %f %f\n", battery->energyStorage, battery->energyProductionUsed, battery->energyConsumptionAmount);
    //printf("Temperature: %f %f %f\n", temperatureRoot->temperature, generator->temperature, generatorRadiators[0]->temperature);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) currentView = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) currentView = 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) currentView = 2;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) currentView = 3;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) currentView = 4;
}

void PlayerVessel::preRender(sf::RenderTarget& window) {}
void PlayerVessel::render(sf::RenderTarget& window)
{
    if (currentView == 0 || currentView == 1)
    {
        radar->render(window);
        renderComponentsOfWindow(window, RADAR_WINDOW);

        if (autoPilot->targetPlanet)
        {
            drawText(window, 20, 230, 0.4, autoPilot->targetPlanet->getName() + " " + to_string(int(autoPilot->targetDistance)) + " " + to_string(int(autoPilot->targetPlanet->hillSphereRadius())), align_left);

            sf::VertexArray a(sf::Lines, 2);
            a[0].position = sf::Vector2f(160, 120) + autoPilot->targetVelocity / 10.0f;
            a[1].position = sf::Vector2f(160, 120) + velocity / 10.0f;
            window.draw(a);
        }
    }

    if (currentView == 0 || currentView == 2)
    {
        renderComponentsOfWindow(window, ENERGY_GRID_WINDOW);
        grid->render(window);
    }

    if (currentView == 0 || currentView == 3)
        renderComponentsOfWindow(window, TEMPERATURE_OVERVIEW_WINDOW);
    
    if (currentView == 4)
    {
        window.pushGLStates();
        
        renderSpace(sf::Vector3f(getPosition().x, getPosition().y, 0), getRotation(), 0, angularVelocity * 0.07);

        window.popGLStates();
    }
}

void PlayerVessel::postRender(sf::RenderTarget& window)
{
    //Draw full screen temperature overlay when we are overheating.
    if (capsule->temperature > CrewCapsule::warningMaxTemperature)
    {
        sf::RectangleShape fullScreenRed(sf::Vector2f(320, 240));
        float alpha = 200;
        if (capsule->temperature < CrewCapsule::maxTemperature)
            alpha = (capsule->temperature - CrewCapsule::warningMaxTemperature) / (CrewCapsule::maxTemperature - CrewCapsule::warningMaxTemperature) * 200;
        fullScreenRed.setFillColor(sf::Color(255, 0, 0, alpha));
        window.draw(fullScreenRed);
    }

    //Draw full screen temperature overlay when we are freezing.
    if (capsule->temperature < CrewCapsule::warningMinTemperature)
    {
        sf::RectangleShape fullScreenBlack(sf::Vector2f(320, 240));
        float alpha = 255;
        if (capsule->temperature > CrewCapsule::minTemperature)
            alpha = (CrewCapsule::warningMinTemperature - capsule->temperature) / (CrewCapsule::warningMinTemperature - CrewCapsule::minTemperature) * 255;
        fullScreenBlack.setFillColor(sf::Color(64, 64, 255, alpha));
        window.draw(fullScreenBlack);
    }

    //Draw full screen blackout overlay when we are blacking out due to pressure loss.
    if (capsule->internalPressure < CrewCapsule::warningPressure)
    {
        sf::RectangleShape fullScreenBlack(sf::Vector2f(320, 240));
        float alpha = 255;
        if (capsule->internalPressure > CrewCapsule::minPressure)
            alpha = (CrewCapsule::warningPressure - capsule->internalPressure) / (CrewCapsule::warningPressure - CrewCapsule::minPressure) * 255;
        fullScreenBlack.setFillColor(sf::Color(0, 0, 0, alpha));
        window.draw(fullScreenBlack);
    }

    //Draw full screen blackout overlay when we are blacking out because of loss of oxygen.
    if (capsule->o2concentration < CrewCapsule::warningO2concentration)
    {
        sf::RectangleShape fullScreenBlack(sf::Vector2f(320, 240));
        float alpha = 255;
        if (capsule->o2concentration > CrewCapsule::minO2concentration)
            alpha = (CrewCapsule::warningO2concentration - capsule->o2concentration) / (CrewCapsule::warningO2concentration - CrewCapsule::minO2concentration) * 255;
        fullScreenBlack.setFillColor(sf::Color(0, 0, 0, alpha));
        window.draw(fullScreenBlack);
    }

    //Draw full screen blackout overlay when we are blacking out because of loss of oxygen.
    if (capsule->co2concentration > CrewCapsule::warningCO2concentration)
    {
        sf::RectangleShape fullScreenBlack(sf::Vector2f(320, 240));
        float alpha = 255;
        if (capsule->co2concentration < CrewCapsule::maxCO2concentration)
            alpha = (capsule->co2concentration - CrewCapsule::warningCO2concentration) / (CrewCapsule::maxCO2concentration - CrewCapsule::warningCO2concentration) * 255;
        fullScreenBlack.setFillColor(sf::Color(0, 0, 0, alpha));
        window.draw(fullScreenBlack);
    }
}
