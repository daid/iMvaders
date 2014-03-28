#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SFML/Graphics.hpp>

#include <map>
#include <vector>

class TextureManager;
extern TextureManager textureManager;
struct TextureData
{
    sf::Texture texture;
    std::vector<sf::IntRect> sprites;
};
class TextureManager
{
private:
    std::map<std::string, TextureData> textureMap;
public:
    TextureManager();
    ~TextureManager();
    
    void setTexture(sf::Sprite& sprite, std::string name, unsigned int spriteIndex = 0);
    const sf::IntRect& getSpriteRect(std::string name, unsigned int spriteIndex = 0);

    sf::Texture* getTexture(std::string name);
private:
    void loadTexture(std::string name);
};

#endif//TEXTURE_MANAGER_H
