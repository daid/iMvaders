#include <stdio.h>

#include "postProcessManager.h"

PostProcessorManager postProcessorManager;

void PostProcessorManager::triggerPostProcess(const char* name, float value)
{
    if (postProcessorMap.find(name) == postProcessorMap.end())
        postProcessorMap[name] = new PostProcessor(name);
    postProcessorMap[name]->trigger(value);
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
                float w = window.getView().getViewport().width * window.getSize().x;
                float h = window.getView().getViewport().height * window.getSize().y;
                backBuffer.create(w, h, false);
                backBuffer.setRepeated(true);
                backBuffer.setView(sf::View(sf::Vector2f(160,120), sf::Vector2f(320, 240)));
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
            sf::Sprite backBufferSprite(backBuffer.getTexture());
            backBufferSprite.setScale(320.0/backBuffer.getSize().x, 240.0/backBuffer.getSize().y);
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
        value -= delta;
}
