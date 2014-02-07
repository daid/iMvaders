#ifndef SPACE_RENDERER_H
#define SPACE_RENDERER_H

#include <SFML/Graphics.hpp>
#include <vector>

class NebulaInfo
{
public:
    sf::Vector3f vector;
    std::string textureName;
};
class RenderInfo
{
public:
    sf::Vector2f cameraPosition;
    float yaw, pitch, roll;
    
    float objectDepth;
    int renderScale;
};

extern sf::Shader* lightShader;
extern std::vector<NebulaInfo> nebulaInfo;

void renderSpace(sf::Vector3f cameraPosition, float yaw, float pitch, float roll);

#endif//SPACE_RENDERER_H
