#ifndef GUI_H
#define GUI_H

#include "P.h"

void renderComponentsOfWindow(sf::RenderTarget& window, int windowID);

class GuiComponent : public virtual PObject
{
public:
    P<PObject> owner;
    int windowID;
    sf::FloatRect rect;
    sf::Color color;
    std::string caption;

    GuiComponent(P<PObject> owner, int windowID, sf::FloatRect rect);
    
    GuiComponent* setPosition(float x, float y);
    GuiComponent* setColor(sf::Color color);
    GuiComponent* setCaption(std::string str);
    
    virtual void render(sf::RenderTarget& window) = 0;
};

class GuiButton : public GuiComponent
{
public:
    bool& value;
    
    GuiButton(P<PObject> owner, bool& value, int windowID, sf::FloatRect rect);
    
    virtual void render(sf::RenderTarget& window);
};

class GuiToggle : public GuiComponent
{
public:
    bool& value;
    
    GuiToggle(P<PObject> owner, bool& value, int windowID, sf::FloatRect rect);
    
    virtual void render(sf::RenderTarget& window);
};

class GuiGauge : public GuiComponent
{
public:
    float& value;
    float minValue, maxValue;
    std::string postfix;
    
    GuiGauge(P<PObject> owner, float& value, float minValue, float maxValue, int windowID, sf::FloatRect rect);
    
    GuiGauge* setPostfix(std::string std);
    
    virtual void render(sf::RenderTarget& window);
};

class GuiSlider : public GuiComponent
{
public:
    float& value;
    float minValue, maxValue;
    
    GuiSlider(P<PObject> owner, float& value, float minValue, float maxValue, int windowID, sf::FloatRect rect);
    
    virtual void render(sf::RenderTarget& window);
};

bool drawButton(sf::RenderTarget& window, float x, float y, float w, float h, sf::Color color);
bool drawToggle(sf::RenderTarget& window, float x, float y, float w, float h, bool& value);

#endif//GUI_H
