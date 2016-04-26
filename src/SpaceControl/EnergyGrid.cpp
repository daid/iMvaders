#include "EnergyGrid.h"
#include "textureManager.h"
#include "textDraw.h"
#include "gui.h"
#include "engine.h"
#include "input.h"

EnergyConsumer::EnergyConsumer(P<EnergyGrid> grid, const char* name, float idleEnergyConsumption, float powerupTimeRequired)
: energyConsumerName(name), energyConsumptionRequest(0), energyConsumptionAmount(0), idleEnergyConsumption(idleEnergyConsumption), powerupTimeRequired(powerupTimeRequired), powerupEnergyLevel(0)
{
    wantOnline = true;
    grid->consumers.push_back(this);

    if (powerupTimeRequired > 0)
        wantOnlineGui = new GuiToggle(this, wantOnline, ENERGY_GRID_WINDOW, sf::FloatRect(0, 0, 8, 8));
    
    powerupEnergyLevel = powerupTimeRequired * idleEnergyConsumption * (1.0f - idleEnergyDisabledDrainFactor);
}

bool EnergyConsumer::isOnline()
{
    return powerupEnergyLevel >= powerupTimeRequired * idleEnergyConsumption * (1.0f - idleEnergyDisabledDrainFactor);
}

float EnergyConsumer::onlineChargeLevel()
{
    if (powerupTimeRequired == 0.0)
        return 1.0;
    return powerupEnergyLevel / (powerupTimeRequired * idleEnergyConsumption * (1.0f - idleEnergyDisabledDrainFactor));
}

EnergyProducer::EnergyProducer(P<EnergyGrid> grid, const char* name)
: energyProductionAmount(0), energyProductionUsed(0), energyProducerName(name)
{
    grid->producers.push_back(this);
}

void EnergyProducer::toggleLink(P<EnergyConsumer> consumer)
{
    if (links.has(consumer))
        links.remove(consumer);
    else
        links.push_back(consumer);
}

void EnergyGrid::update(float delta)
{
    foreach(EnergyConsumer, c, consumers)
    {
        c->powerupEnergyLevel -= (c->idleEnergyConsumption * EnergyConsumer::idleEnergyDisabledDrainFactor) * delta;
        if (c->powerupEnergyLevel < 0.0)
            c->powerupEnergyLevel = 0.0;
        c->energyConsumptionAmount = 0;
    }
    foreach(EnergyProducer, p, producers)
    {
        p->energyProductionUsed = 0;
        float productionAmount = p->energyProductionAmount;
        foreach(EnergyConsumer, c, p->links)
        {
            if (c->wantOnline)
            {
                float used = std::min(productionAmount, c->idleEnergyConsumption);
                productionAmount -= used;
                c->powerupEnergyLevel += used * delta;
                if (c->powerupEnergyLevel > c->powerupTimeRequired * c->idleEnergyConsumption * (1.0f - EnergyConsumer::idleEnergyDisabledDrainFactor))
                    c->powerupEnergyLevel = c->powerupTimeRequired * c->idleEnergyConsumption * (1.0f - EnergyConsumer::idleEnergyDisabledDrainFactor) + (c->idleEnergyConsumption / 100.0f);
            }
        }
        while(productionAmount > 0)
        {
            int consumerCount = 0;
            foreach(EnergyConsumer, c, p->links)
            {
                if (c->isOnline() && c->energyConsumptionAmount < c->energyConsumptionRequest)
                    consumerCount++;
            }
            float usedAmount = 0.0;
            foreach(EnergyConsumer, c, p->links)
            {
                if (c->isOnline() && c->energyConsumptionAmount < c->energyConsumptionRequest)
                {
                    float a = productionAmount / float(consumerCount);
                    if (c->energyConsumptionAmount + a < c->energyConsumptionRequest)
                    {
                        c->energyConsumptionAmount += a;
                    }else{
                        a = c->energyConsumptionRequest - c->energyConsumptionAmount;
                        c->energyConsumptionAmount = c->energyConsumptionRequest;
                    }
                    usedAmount += a;
                }
            }
            if (usedAmount == 0.0)
                break;
            productionAmount -= usedAmount;
        }
        p->energyProductionUsed = p->energyProductionAmount - productionAmount;
    }
}

P<EnergyProducer> producerSelect;
P<EnergyConsumer> consumerSelect;
void EnergyGrid::render(sf::RenderTarget& window)
{
    int n = 0;
    foreach(EnergyProducer, p, producers)
    {
        if (drawButton(window, 70 - 4, 10 + 10 * n - 4, 8, 8, sf::Color::Black))
        {
            producerSelect = p;
            if (consumerSelect)
            {
                producerSelect->toggleLink(consumerSelect);
                producerSelect = NULL;
                consumerSelect = NULL;
            }
        }
        sf::Sprite sprite;
        textureManager.setTexture(sprite, "RadarBlip");
        sprite.setScale(0.2, 0.2);
        sprite.setPosition(70, 10 + 10 * n);
        window.draw(sprite);
        drawText(window, 65, 7 + 10 * n, 0.5, p->energyProducerName, align_right);
        
        foreach(EnergyConsumer, c, p->links)
        {
            int m = 0;
            foreach(EnergyConsumer, c1, consumers)
            {
                if (c == c1)
                {
                    sf::VertexArray a(sf::Lines, 2);
                    a[0].position = sf::Vector2f(70, 10 + 10 * n);
                    a[1].position = sf::Vector2f(240, 10 + 10 * m);
                    window.draw(a);
                }
                m++;
            }
        }
/*
        if (p->energyProductionAmount > 0.0)
        {
            sf::RectangleShape shape(sf::Vector2f(50 * p->energyProductionUsed / p->energyProductionAmount, 2));
            shape.setPosition(20, 13 + 10 * n);
            window.draw(shape);
        }
*/
        n++;
    }

    n = 0;
    foreach(EnergyConsumer, c, consumers)
    {
        if (drawButton(window, 240 - 4, 10 + 10 * n - 4, 8, 8, sf::Color::Black))
        {
            consumerSelect = c;
            if (producerSelect)
            {
                producerSelect->toggleLink(consumerSelect);
                producerSelect = NULL;
                consumerSelect = NULL;
            }
        }
        
        sf::Sprite sprite;
        textureManager.setTexture(sprite, "RadarBlip");
        sprite.setScale(0.2, 0.2);
        sprite.setPosition(240, 10 + 10 * n);
        window.draw(sprite);
        drawText(window, 255, 7 + 10 * n, 0.5, c->energyConsumerName, align_left);

        if (c->onlineChargeLevel() < 1.0)
        {
            sf::RectangleShape shape(sf::Vector2f(50 * c->onlineChargeLevel(), 2));
            shape.setFillColor(sf::Color::Red);
            shape.setPosition(255, 13 + 10 * n);
            window.draw(shape);
        }
        if (c->energyConsumptionRequest > 0.0)
        {
            sf::RectangleShape shape(sf::Vector2f(50 * c->energyConsumptionAmount / c->energyConsumptionRequest, 2));
            shape.setPosition(255, 13 + 10 * n);
            window.draw(shape);
        }
        
        if (c->wantOnlineGui)
            c->wantOnlineGui->setPosition(245, 6 + n * 10);
        //if (c->powerupTimeRequired > 0)
        //    drawToggle(window, 245, 6 + n * 10, 8, 8, c->wantOnline);
        
        n++;
    }

    sf::Vector2f p = InputHandler::getMousePos();
    
    sf::RectangleShape shape(sf::Vector2f(3, 3));
    shape.setPosition(p);
    window.draw(shape);
}
