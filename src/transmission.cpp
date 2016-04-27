#include <math.h>

#include "transmission.h"
#include "textDraw.h"
#include "player.h"
#include "textureManager.h"
#include "engine.h"
#include "main.h"

REGISTER_SCRIPT_CLASS(Transmission)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(Transmission, setFace);
    REGISTER_SCRIPT_CLASS_FUNCTION(Transmission, setText);
    REGISTER_SCRIPT_CLASS_FUNCTION(Transmission, top);
    REGISTER_SCRIPT_CLASS_FUNCTION(Transmission, bottom);
    REGISTER_SCRIPT_CLASS_CALLBACK(Transmission, transmissionDone);
}

Transmission::Transmission()
: GameEntity(hudLayer)
{
    faceName = "";
    text = "NO TEXT SET";
    transmissionTime = 0;
    positionY = 160;
}
void Transmission::setFace(const char* faceName)
{
    this->faceName = faceName;
}

void Transmission::setText(const char* text)
{
    this->text = text;
}

void Transmission::top()
{
    positionY = 20;
}

void Transmission::bottom()
{
    positionY = 160;
}

void Transmission::render(sf::RenderTarget& window)
{
    sf::RectangleShape transmissionShape(sf::Vector2f(280, 62));
    transmissionShape.setPosition(20, positionY);
    transmissionShape.setFillColor(sf::Color(0, 0, 0, 128));
    transmissionShape.setOutlineColor(sf::Color::White);
    transmissionShape.setOutlineThickness(1);
    window.draw(transmissionShape);
    if (faceName.size() > 0)
    {
        face.setPosition(21+30, positionY+1+30);
        window.draw(face);
        drawText(window, 23 + 60, positionY+1, text.substr(0, int(transmissionTime * charsPerSecond)), align_left);
    }else{
        drawText(window, 23, positionY+1, text.substr(0, int(transmissionTime * charsPerSecond)), align_left);
    }
}

void Transmission::update(float delta)
{
    transmissionTime += delta;
    
    if (faceName.size() > 0)
    {
        if (transmissionTime * charsPerSecond < text.length())
            textureManager.setTexture(face, faceName.c_str(), fmodf(transmissionTime * charsPerSecond, 2.0f) < 1.0f ? 1 : 0);
        else
            textureManager.setTexture(face, faceName.c_str(), 0);
    }

    if (transmissionTime > float(text.length()) / charsPerSecond + extraDelayBeforeDone)
    {
        transmissionDone();
        destroy();
        return;
    }
    for(int n=0; n<MAX_PLAYERS; n++)
    {
        P<PlayerController> pc = engine->getObject("playerController" + string(n + 1));
        if (pc->button(skipButton))
        {
            transmissionDone();
            destroy();
            return;
        }
    }
}
