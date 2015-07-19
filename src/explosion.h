#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "gameEntity.h"

class Particle
{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    
    Particle(sf::Vector2f position, sf::Vector2f velocity)
    : position(position), velocity(velocity)
    {}
};

class Explosion : public GameEntity
{
private:
    sf::Sound sound;
    std::vector<Particle> particles;
    float life;
    static constexpr float lifeTime = 0.5;
public:
    Explosion(sf::Vector2f position, float radius, sf::Vector2f addedVelocity = sf::Vector2f(0, 0));
    virtual ~Explosion();
    
    virtual void update(float delta);
    
    virtual void render(sf::RenderTarget& window);
};

#endif//EXPLOSION_H
