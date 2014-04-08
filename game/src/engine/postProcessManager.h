#ifndef POST_PROCESS_MANAGER_H
#define POST_PROCESS_MANAGER_H

#include <SFML/Graphics.hpp>

#include "stringImproved.h"
#include "Updatable.h"

class PostProcessor;
class PostProcessorManager;
extern PostProcessorManager postProcessorManager;
class PostProcessorManager
{
private:
    std::map<string, PostProcessor*> postProcessorMap;
    sf::RenderTexture backBuffer[2];
    sf::Vector2i virtualSize;
public:
    sf::RenderTarget& getPrimaryRenderTarget(sf::RenderTarget& window);
    void postProcessRendering(sf::RenderTarget& window);
    
    void triggerPostProcess(const char* name, float value);
    
    void setVirtualSize(sf::Vector2i virtualSize) { this->virtualSize = virtualSize; }
};

class PostProcessor : public Updatable
{
    friend class PostProcessorManager;
private:
    sf::Shader shader;
    float value;
    float speedFactor;

    PostProcessor(const char* name);
    virtual ~PostProcessor() {}
    
    void trigger(float f);
    void process(sf::RenderTarget& target, sf::Texture& source);
    bool active();
public:
    virtual void update(float delta);
};

#endif//POST_PROCESS_MANAGER_H
