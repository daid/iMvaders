#ifndef RADAR_DISPLAY_H
#define RADAR_DISPLAY_H

#define RADAR_WINDOW 0

class RadarDisplay: public EnergyConsumer, public Updatable
{
    SpaceObject* owner;
    
    sf::Sprite staticDisplay;
    sf::Sprite radarGrid;
    sf::Sprite radarCutoff;
public:
    float radarDistance;
    float viewDistance;
    float directionalDistance;
    float directionalAngle;
    float directionalWidth;
    
    RadarDisplay(EnergyGrid* grid, SpaceObject* owner)
    : EnergyConsumer(grid, "Radar", 10, 5), owner(owner)
    {
        textureManager.setTexture(staticDisplay, "noise");
        staticDisplay.setTextureRect(sf::IntRect(0, 0, 2048, 2048));
        staticDisplay.setOrigin(sf::Vector2f(1024, 1024));
        staticDisplay.setScale(2.0, 2.0);
        
        textureManager.setTexture(radarGrid, "RadarBG");
        radarGrid.setScale(2.0, 2.0);
        textureManager.setTexture(radarCutoff, "radarCutoff");
        
        radarDistance = 1024;
        viewDistance = 512;
        directionalDistance = 0;
        directionalAngle = 0;
        directionalWidth = 10;
    }
    
    virtual void update(float delta)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add) || sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
            viewDistance *= powf(0.5, delta);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) || sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))
            viewDistance *= powf(2.0, delta);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Multiply))
            radarDistance *= powf(0.5, delta);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Divide))
            radarDistance *= powf(2.0, delta);

        energyConsumptionRequest = 25 * powf(radarDistance / 1024.0f, 2.0);
        energyConsumptionRequest += 35 * powf(directionalDistance / 1024.0f, 2.0) * directionalWidth / 360;

        if (viewDistance < 256.0f)
            viewDistance = 256.0f;
    }

    virtual void render(sf::RenderTarget& window)
    {
        sf::View oldView = window.getView();
        
        float ratio = float(oldView.getSize().y) / float(oldView.getSize().x);
        window.setView(sf::View(owner->getPosition(), sf::Vector2f(viewDistance * 2, viewDistance * 2 * ratio)));
        
        radarGrid.setTextureRect(sf::IntRect(0, 0, int((radarDistance+63)/64.0)*64*2, int((radarDistance+63)/64.0)*64*2));
        radarGrid.setOrigin(sf::Vector2f(radarGrid.getTextureRect().width / 2, radarGrid.getTextureRect().width / 2));
        radarGrid.setPosition(sf::Vector2f(sf::Vector2i(owner->getPosition() / 128.0f) * 128));
        window.draw(radarGrid);
        radarCutoff.setPosition(owner->getPosition());
        radarCutoff.setScale(radarDistance / 256.0f, radarDistance / 256.0f);
        window.draw(radarCutoff);
        if (radarDistance < viewDistance)
        {
            sf::RectangleShape rect(sf::Vector2f(viewDistance * 2, viewDistance - radarDistance));
            rect.setFillColor(sf::Color::Black);
            rect.setPosition(owner->getPosition() + sf::Vector2f(-viewDistance,-viewDistance));
            window.draw(rect);
            rect.setPosition(owner->getPosition() + sf::Vector2f(-viewDistance, radarDistance));
            window.draw(rect);
            rect.setSize(sf::Vector2f(viewDistance - radarDistance, viewDistance * 2));
            rect.setPosition(owner->getPosition() + sf::Vector2f(-viewDistance,-viewDistance));
            window.draw(rect);
            rect.setPosition(owner->getPosition() + sf::Vector2f( radarDistance,-viewDistance));
            window.draw(rect);
        }
        
        if (directionalDistance > radarDistance)
        {
            const int angleCount = 5;
            sf::VertexArray array(sf::TrianglesFan, 1 + angleCount);
            array[0].position = owner->getPosition();
            array[0].texCoords = array[0].position / 2.0f;
            for(int n=0; n<angleCount; n++)
            {
                array[n+1].position = owner->getPosition() + directionalDistance * sf::vector2FromAngle(directionalAngle - directionalWidth / 2.0f + directionalWidth * float(n) / float(angleCount-1));
                array[n+1].texCoords = array[n+1].position / 2.0f;
            }
            window.draw(array, sf::RenderStates(textureManager.getTexture("RadarBG")));
        }

        //Draw shadows of the planets.
        foreach(Planet, p, planetList)
        {
            sf::Vector2f diff = owner->getPosition() - p->getPosition();
            float a0 = sf::vector2ToAngle(diff);
            float a1 = acosf(p->getRadius() / sf::length(diff)) / M_PI * 180.0;
            
            sf::VertexArray array(sf::TrianglesStrip, 8);
            array[0].position = p->getPosition() + sf::vector2FromAngle(a0 - a1) * p->getRadius();
            array[0].color = sf::Color(0,0,0,0);
            array[1].position = sf::normalize(array[0].position - owner->getPosition()) * viewDistance * 2.0f + owner->getPosition();
            array[1].color = sf::Color(0,0,0,0);

            array[2].position = p->getPosition() + sf::vector2FromAngle(a0 - a1) * p->getRadius() * 0.9f;
            array[2].color = sf::Color(0,0,0,255);
            array[3].position = sf::normalize(array[2].position - owner->getPosition()) * viewDistance * 2.0f + owner->getPosition();
            array[3].color = sf::Color(0,0,0,255);

            array[4].position = p->getPosition() + sf::vector2FromAngle(a0 + a1) * p->getRadius() * 0.9f;
            array[4].color = sf::Color(0,0,0,255);
            array[5].position = sf::normalize(array[4].position - owner->getPosition()) * viewDistance * 2.0f + owner->getPosition();
            array[5].color = sf::Color(0,0,0,255);

            array[6].position = p->getPosition() + sf::vector2FromAngle(a0 + a1) * p->getRadius();
            array[6].color = sf::Color(0,0,0,0);
            array[7].position = sf::normalize(array[6].position - owner->getPosition()) * viewDistance * 2.0f + owner->getPosition();
            array[7].color = sf::Color(0,0,0,0);
            window.draw(array);
        }
        
        if (directionalDistance > radarDistance)
        {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = owner->getPosition();
            line[0].color = sf::Color(255, 255, 255, 64);
            line[1].position = owner->getPosition() + directionalDistance * sf::vector2FromAngle(directionalAngle - directionalWidth / 2.0f);
            line[1].color = sf::Color(255, 255, 255, 32);
            window.draw(line);
            line[1].position = owner->getPosition() + directionalDistance * sf::vector2FromAngle(directionalAngle + directionalWidth / 2.0f);
            window.draw(line);
        }
        
        Planet* majorPlanet = NULL;
        Planet* minorPlanet = NULL;
        foreach(Planet, planet, planetList)
        {
            if (majorPlanet == NULL || sf::length(majorPlanet->gravity(owner->getPosition())) < sf::length(planet->gravity(owner->getPosition())))
                majorPlanet = *planet;
        }
        foreach(Planet, planet, planetList)
        {
            if (majorPlanet != *planet && (minorPlanet == NULL || sf::length(minorPlanet->gravity(owner->getPosition())) < sf::length(planet->gravity(owner->getPosition()))))
                minorPlanet = *planet;
        }
        float gMajor = sf::length(majorPlanet->gravity(owner->getPosition()));
        float gMinor = 0;
        if (minorPlanet)
            gMinor = sf::length(minorPlanet->gravity(owner->getPosition()));
        sf::VertexArray majorArray(sf::LinesStrip, 100);
        sf::VertexArray minorArray(sf::LinesStrip, 100);
        sf::Vector2f p = owner->getPosition();
        sf::Vector2f v = owner->velocity;
        float timeOffset = 0.0;
        for(unsigned int n=0; n<100; n++)
        {
            majorArray[n].position = p - majorPlanet->predictPositionAtDelta(timeOffset) + majorPlanet->getPosition();
            if (minorPlanet)
                minorArray[n].position = p - minorPlanet->predictPositionAtDelta(timeOffset) + minorPlanet->getPosition();
            majorArray[n].color = sf::Color(255, 255, 0, 255 * (99 - n) / 100);
            minorArray[n].color = sf::Color(255, 255, 0, 255 * (99 - n) / 100 * (gMinor / gMajor));
            sf::Vector2f a;
            foreach(Planet, planet, planetList)
                a += planet->gravity(p, timeOffset);
            float f = sf::length((v + a * 10.0f) - majorPlanet->velocity);
            f = std::max(f, sf::length((v + a * 10.0f)));
            if (f < 1.0f) f = 1.0f;
            f = (viewDistance / 32.0f) / f;
            timeOffset += f;
            v += a * f;
            p += v * f;
        }
        window.draw(majorArray);
        if (minorPlanet)
            window.draw(minorArray);
        
        owner->sprite.setScale(viewDistance / 512.0f, viewDistance / 512.0f);
        foreach(SpaceObject, o, spaceObjectList)
        {
            if (checkVisible(o))
                o->renderOnRadar(window);
        }
    
        //Render static on top of the radar
        window.setView(oldView);
        staticDisplay.setPosition(sf::Vector2f(random(-512, 512), random(-512, 512)));
        if (isOnline())
        {
            if (energyConsumptionAmount > energyConsumptionRequest)
                energyConsumptionAmount = energyConsumptionRequest;
            staticDisplay.setColor(sf::Color(255, 255, 255, 200 * (1.0 - energyConsumptionAmount / energyConsumptionRequest)));
            window.draw(staticDisplay);
        }else{
            staticDisplay.setColor(sf::Color(255, 255, 255, 255));
            window.draw(staticDisplay);
            if (powerupEnergyLevel == 0.0)
            {
                drawText(window, 160, 200, 0.5, "Radar system offline", align_center);
            }else{
                drawText(window, 160, 200, 0.5, "Booting radar system", align_center);
                sf::RectangleShape shape(sf::Vector2f(100 * onlineChargeLevel(), 10));
                shape.setPosition(110, 210);
                window.draw(shape);
            }
        }
    }
    
    bool checkVisible(P<SpaceObject> o)
    {
        if (o->alwaysVisible)
            return true;
        if (owner->getPosition() - o->getPosition() < radarDistance * energyConsumptionAmount / energyConsumptionRequest)
            return checkLineOfSight(owner->getPosition(), o->getPosition());
        if (owner->getPosition() - o->getPosition() < directionalDistance * energyConsumptionAmount / energyConsumptionRequest)
        {
            float angleDiff = sf::vector2ToAngle(o->getPosition() - owner->getPosition()) - directionalAngle;
            while (angleDiff > 180) angleDiff -= 360;
            while (angleDiff < -180) angleDiff += 360;
            if (abs(angleDiff) < directionalWidth / 2.0f)
                return checkLineOfSight(owner->getPosition(), o->getPosition());
        }
        return false;
    }
};

#endif//RADAR_DISPLAY_H
