#include <SFML/Graphics.hpp>
#include "gui.h"
#include "input.h"
#include "engine.h"
#include "textDraw.h"
#include "stringUtils.h"

PVector<GuiComponent> guiComponentList;

void renderComponentsOfWindow(sf::RenderTarget& window, int windowID)
{
    foreach(GuiComponent, c, guiComponentList)
    {
        if (!c->owner)
            c->destroy();
        else if (windowID == c->windowID)
            c->render(window);
    }
}

GuiComponent::GuiComponent(P<PObject> owner, int windowID, sf::FloatRect rect)
: owner(owner), windowID(windowID), rect(rect)
{
    guiComponentList.push_back(this);
    color = sf::Color::White;
}

GuiComponent* GuiComponent::setPosition(float x, float y)
{
    rect.left = x;
    rect.top = y;
    return this;
}

GuiComponent* GuiComponent::setColor(sf::Color color)
{
    this->color = color;
    return this;
}

GuiComponent* GuiComponent::setCaption(std::string str)
{
    caption = str;
    return this;
}

GuiButton::GuiButton(P<PObject> owner, bool& value, int windowID, sf::FloatRect rect)
: GuiComponent(owner, windowID, rect), value(value)
{
}

void GuiButton::render(sf::RenderTarget& window)
{
    sf::RectangleShape button(sf::Vector2f(rect.width, rect.height));
    button.setPosition(rect.left, rect.top);
    button.setFillColor(color);
    if (value)
        button.setOutlineColor(sf::Color::White);
    else
        button.setOutlineColor(sf::Color::Black);
    button.setOutlineThickness(0.5);
    window.draw(button);

    P<InputHandler> input = engine->getObject("inputHandler");
    sf::Vector2f p = input->getMousePos();
    value = (rect.contains(p) && input->mouseIsDown(0));

    drawText(window, rect.left, rect.top - 4, 0.3, caption, align_left);
}

GuiToggle::GuiToggle(P<PObject> owner, bool& value, int windowID, sf::FloatRect rect)
: GuiComponent(owner, windowID, rect), value(value)
{
}

void GuiToggle::render(sf::RenderTarget& window)
{
    sf::RectangleShape button(sf::Vector2f(rect.width, rect.height));
    button.setPosition(rect.left, rect.top);
    if (value)
        button.setFillColor(sf::Color(128,255,128));
    else
        button.setFillColor(sf::Color(255,128,128));
    window.draw(button);

    P<InputHandler> input = engine->getObject("inputHandler");
    sf::Vector2f p = input->getMousePos();
    if (rect.contains(p) && input->mouseIsPressed(0))
        value = !value;

    drawText(window, rect.left, rect.top - 4, 0.3, caption, align_left);
}

GuiGauge::GuiGauge(P<PObject> owner, float& value, float minValue, float maxValue, int windowID, sf::FloatRect rect)
: GuiComponent(owner, windowID, rect), value(value), minValue(minValue), maxValue(maxValue)
{
}

GuiGauge* GuiGauge::setPostfix(std::string str)
{
    postfix = str;
    return this;
}

void GuiGauge::render(sf::RenderTarget& window)
{
    sf::RectangleShape gauge(sf::Vector2f(rect.width, rect.height));
    gauge.setPosition(rect.left, rect.top);
    gauge.setFillColor(sf::Color(color.r, color.g, color.b, 64));
    window.draw(gauge);

    float f = (value - minValue) / (maxValue - minValue);
    if (f < 0.0f)
        f = 0.0f;
    if (f > 1.0f)
        f = 1.0f;
    gauge.setSize(sf::Vector2f(rect.width * f, rect.height));
    gauge.setFillColor(color);
    window.draw(gauge);

    drawText(window, rect.left, rect.top - 4, 0.3, caption, align_left);
    if (postfix.length() > 0)
        drawText(window, rect.left, rect.top, 0.3, to_string(int(value)) + postfix, align_left);
}

GuiSlider::GuiSlider(P<PObject> owner, float& value, float minValue, float maxValue, int windowID, sf::FloatRect rect)
: GuiComponent(owner, windowID, rect), value(value), minValue(minValue), maxValue(maxValue)
{
}

void GuiSlider::render(sf::RenderTarget& window)
{
    sf::RectangleShape slider(sf::Vector2f(rect.width, rect.height));
    slider.setPosition(rect.left, rect.top);
    slider.setFillColor(sf::Color(color.r, color.g, color.b, 64));
    window.draw(slider);

    slider.setSize(sf::Vector2f(rect.height, rect.height));
    float f = (value - minValue) / (maxValue - minValue);
    if (f < 0.0f)
        f = 0.0f;
    if (f > 1.0f)
        f = 1.0f;
    slider.setPosition(rect.left + (rect.width - rect.height) * f, rect.top);
    slider.setFillColor(color);
    window.draw(slider);

    P<InputHandler> input = engine->getObject("inputHandler");
    sf::Vector2f p = input->getMousePos();
    if (rect.contains(p) && input->mouseIsDown(0))
    {
        f = std::min(1.0f, std::max(0.0f, (p.x - rect.left - rect.height / 2) / (rect.width - rect.height)));
        value = minValue + (maxValue - minValue) * f;
    }

    drawText(window, rect.left, rect.top - 3, 0.3, caption, align_left);
}

bool drawButton(sf::RenderTarget& window, float x, float y, float w, float h, sf::Color color)
{
    sf::RectangleShape button(sf::Vector2f(w, h));
    button.setPosition(x, y);
    button.setFillColor(color);
    window.draw(button);
    P<InputHandler> input = engine->getObject("inputHandler");
    sf::Vector2f p = input->getMousePos();
    if (p.x >= x && p.y >= y && p.x <= x + w && p.y <= y + h && input->mouseIsPressed(0))
    {
        return true;
    }
    return false;
}

bool drawToggle(sf::RenderTarget& window, float x, float y, float w, float h, bool& value)
{
    if (drawButton(window, x, y, w, h, value ? sf::Color::Green : sf::Color::Red))
    {
        value = !value;
        return true;
    }
    return false;
}
