#include <stdio.h>

#include "postProcessManager.h"

PostProcessorManager postProcessorManager;

void PostProcessorManager::triggerPostProcess(const char* name, float value)
{
    if (!sf::Shader::isAvailable())
        return;
    if (postProcessorMap.find(name) == postProcessorMap.end())
        postProcessorMap[name] = new PostProcessor(name);
    postProcessorMap[name]->trigger(value);
}

int powerOfTwo(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

sf::RenderTarget& PostProcessorManager::getPrimaryRenderTarget(sf::RenderTarget& window)
{
    for(std::map<sf::String, PostProcessor*>::iterator it = postProcessorMap.begin(); it != postProcessorMap.end(); it++)
    {
        if (it->second->active())
        {
            if (backBuffer.getSize().x < 1)
            {
                //Setup a backBuffer to render the game on. Then we can render the backbuffer back to the main screen with full-screen shader effects
                int w = window.getView().getViewport().width * window.getSize().x;
                int h = window.getView().getViewport().height * window.getSize().y;
                int tw = powerOfTwo(w);
                int th = powerOfTwo(h);
                backBuffer.create(tw, th, false);
                backBuffer.setRepeated(true);
                sf::View view(sf::Vector2f(160,120), sf::Vector2f(320, 240));
                view.setViewport(sf::FloatRect(0, 0, float(w) / float(tw), float(h) / float(th)));
                backBuffer.setView(view);
            }
            return backBuffer;
        }
    }
    return window;
}

void PostProcessorManager::postProcessRendering(sf::RenderTarget& window)
{
    for(std::map<sf::String, PostProcessor*>::iterator it = postProcessorMap.begin(); it != postProcessorMap.end(); it++)
    {
        if (it->second->active())
        {
            backBuffer.display();
            sf::Sprite backBufferSprite(backBuffer.getTexture(), sf::IntRect(0,0, window.getSize().x, window.getSize().y));
            backBufferSprite.setScale(320.0/window.getSize().x, 240.0/window.getSize().y);
            it->second->shader.setParameter("value", it->second->value);
            window.draw(backBufferSprite, &it->second->shader);
        }
    }
}

PostProcessor::PostProcessor(const char* name)
{
    char buffer[128];
    sprintf(buffer, "resources/%s.frag", name);
    if (shader.loadFromFile(buffer, sf::Shader::Fragment))
    {
        printf("Loaded post-processor: %s\n", name);
    }else{
        printf("Failed to load post-processor: %s\n", name);
    }
    value = 0.0;
    speedFactor = 1.0;
}

void PostProcessor::trigger(float value)
{
    this->value = value;
}

bool PostProcessor::active()
{
    return this->value > 0.0;
}

void PostProcessor::update(float delta)
{
    if (value > 0.0)
        value -= delta * speedFactor;
}
