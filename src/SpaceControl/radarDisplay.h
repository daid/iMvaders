#ifndef RADAR_DISPLAY_H
#define RADAR_DISPLAY_H

#include "EnergyGrid.h"
#include "spaceObject.h"

#define RADAR_WINDOW 0

class RadarDisplay: public EnergyConsumer, public Updatable
{
    P<SpaceObject> owner;
    
    sf::Sprite staticDisplay;
    sf::Sprite radarGrid;
    sf::Sprite radarCutoff;
public:
    float radarDistance;
    float viewDistance;
    float directionalDistance;
    float directionalAngle;
    float directionalWidth;
    
    RadarDisplay(P<EnergyGrid> grid, P<SpaceObject> owner);
    
    virtual void update(float delta);

    virtual void render(sf::RenderTarget& window);
    
    bool checkVisible(P<SpaceObject> o);
};

#endif//RADAR_DISPLAY_H
