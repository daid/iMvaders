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
    for(std::map<string, PostProcessor*>::iterator it = postProcessorMap.begin(); it != postProcessorMap.end(); it++)
    {
        if (it->second->active())
        {
            if (backBuffer[0].getSize().x < 1)
            {
                //Setup a backBuffer to render the game on. Then we can render the backbuffer back to the main screen with full-screen shader effects
                int w = window.getView().getViewport().width * window.getSize().x;
                int h = window.getView().getViewport().height * window.getSize().y;
                int tw = powerOfTwo(w);
                int th = powerOfTwo(h);
                sf::View view(sf::Vector2f(virtualSize.x/2,virtualSize.y/2), sf::Vector2f(virtualSize));
                view.setViewport(sf::FloatRect(0, 1.0 - float(h) / float(th), float(w) / float(tw), float(h) / float(th)));

                backBuffer[0].create(tw, th, false);
                backBuffer[0].setRepeated(true);
                backBuffer[0].setSmooth(true);
                backBuffer[0].setView(view);

                backBuffer[1].create(tw, th, false);
                backBuffer[1].setRepeated(true);
                backBuffer[1].setSmooth(true);
                backBuffer[1].setView(view);
            }
            return backBuffer[0];
        }
    }
    return window;
}

void PostProcessorManager::postProcessRendering(sf::RenderTarget& window)
{
    int bufferNr = 0;
    PostProcessor* postProcessor = NULL;
    for(std::map<string, PostProcessor*>::iterator it = postProcessorMap.begin(); it != postProcessorMap.end(); it++)
    {
        if (it->second->active())
        {
            if (postProcessor)
            {
                backBuffer[bufferNr].display();
                sf::Sprite backBufferSprite(backBuffer[bufferNr].getTexture(), sf::IntRect(0, backBuffer[0].getSize().y - window.getView().getViewport().height * window.getSize().y, window.getView().getViewport().width * window.getSize().x, window.getView().getViewport().height * window.getSize().y));
                backBufferSprite.setScale(virtualSize.x/float(backBuffer[0].getSize().x)/backBuffer[0].getView().getViewport().width, virtualSize.y/float(backBuffer[0].getSize().y)/backBuffer[0].getView().getViewport().height);
                if (postProcessor->value >= 0.0)
                    postProcessor->shader.setParameter("value", postProcessor->value);
                postProcessor->shader.setParameter("inputSize", sf::Vector2f(window.getSize().x, window.getSize().y));
                postProcessor->shader.setParameter("textureSize", sf::Vector2f(backBuffer[0].getSize().x, backBuffer[0].getSize().y));
                bufferNr = (bufferNr + 1) % 2;
                backBuffer[bufferNr].draw(backBufferSprite, &postProcessor->shader);
            }
            postProcessor = it->second;
        }
    }
    if (postProcessor)
    {
        backBuffer[bufferNr].display();
        sf::Sprite backBufferSprite(backBuffer[bufferNr].getTexture(), sf::IntRect(0, backBuffer[0].getSize().y - window.getView().getViewport().height * window.getSize().y, window.getView().getViewport().width * window.getSize().x, window.getView().getViewport().height * window.getSize().y));
        backBufferSprite.setScale(virtualSize.x/float(backBuffer[0].getSize().x)/backBuffer[0].getView().getViewport().width, virtualSize.y/float(backBuffer[0].getSize().y)/backBuffer[0].getView().getViewport().height);
        if (postProcessor->value >= -10.0)
            postProcessor->shader.setParameter("value", postProcessor->value);
        postProcessor->shader.setParameter("inputSize", sf::Vector2f(window.getSize().x, window.getSize().y));
        postProcessor->shader.setParameter("textureSize", sf::Vector2f(backBuffer[0].getSize().x, backBuffer[0].getSize().y));
        window.draw(backBufferSprite, &postProcessor->shader);
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
    return this->value > 0.0 || this->value < -10.0;
}

void PostProcessor::update(float delta)
{
    if (value > 0.0)
        value -= delta * speedFactor;
}
