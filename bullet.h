#ifndef BULLET_H
#define BULLET_H

#include <SFML/Audio.hpp>

#include "gameEntity.h"

class Bullet: public GameEntity
{
private:
    sf::Sound sound;
    float speed;
    int type;
public:
    Bullet(sf::Vector2f position, int type, float angle, float speed = 180.0f);
    
    virtual void update(float delta);
    
    virtual void render(sf::RenderTarget& window);
};

#endif//BULLET_H
