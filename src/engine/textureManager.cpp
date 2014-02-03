#include <stdio.h>

#include "textureManager.h"

TextureManager textureManager;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

void TextureManager::setTexture(sf::Sprite& sprite, std::string name, unsigned int spriteIndex)
{
    TextureData& data = textureMap[name];
    if (data.texture.getSize().x < 1)
        loadTexture(name);
    
    if (spriteIndex < data.sprites.size())
    {
        sprite.setTexture(data.texture);
        sprite.setTextureRect(data.sprites[spriteIndex]);
        sprite.setOrigin(data.sprites[spriteIndex].width / 2, data.sprites[spriteIndex].height / 2);
    }else{
        sprite.setTexture(data.texture, true);
        sprite.setOrigin(data.texture.getSize().x / 2, data.texture.getSize().y / 2);
    }
}

sf::Texture* TextureManager::getTexture(std::string name)
{
    TextureData& data = textureMap[name];
    if (data.texture.getSize().x < 1)
        loadTexture(name);
    return &data.texture;
}

const sf::IntRect& TextureManager::getSpriteRect(std::string name, unsigned int spriteIndex)
{
    TextureData& data = textureMap[name];
    if (data.texture.getSize().x < 1)
        loadTexture(name);
    
    if (spriteIndex < data.sprites.size())
        return data.sprites[spriteIndex];
    
    static sf::IntRect noRect;
    return noRect;
}

void TextureManager::loadTexture(std::string name)
{
    TextureData& data = textureMap[name];
    
    char buffer[128];
    sprintf(buffer, "resources/%s.png", name.c_str());
    sf::Image tmpImage;
    if (!tmpImage.loadFromFile(buffer))
    {
        printf("Failed to load: %s\n", buffer);
        data.texture.create(8, 8);
        return;
    }
    for(unsigned int y=0; y<tmpImage.getSize().y; y++)
    {
        for(unsigned int x=0; x<tmpImage.getSize().x; x++)
        {
            if (tmpImage.getPixel(x, y) == sf::Color(255,0,255))
            {
                unsigned int x1=x+1;
                for(; x1<tmpImage.getSize().x; x1++)
                    if (tmpImage.getPixel(x1, y) == sf::Color(255,0,255))
                        break;
                unsigned int y1=y+1;
                for(; y1<tmpImage.getSize().y; y1++)
                    if (tmpImage.getPixel(x, y1) == sf::Color(255,0,255))
                        break;
                if (x1-x > 1 && y1-y > 1 && x1<tmpImage.getSize().x && y1<tmpImage.getSize().y)
                    data.sprites.push_back(sf::IntRect(x+1, y+1, x1-x-1, y1-y-1));
                x = x1-1;
            }
        }
    }
    data.texture.setRepeated(true);
    
    data.texture.loadFromImage(tmpImage);
    printf("Loaded: %s with %d sprites\n", buffer, data.sprites.size());
}
