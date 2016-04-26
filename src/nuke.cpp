#include <math.h>
#include "nuke.h"
#include "vectorUtils.h"
#include "soundManager.h"
#include "textureManager.h"
#include "random.h"
#include "vectorUtils.h"
#include "explosion.h"

Nuke::Nuke(sf::Vector2f position, sf::Vector2f velocity, float size, int owner)
: GameEntity(), Collisionable(size), velocity(velocity), size(size), owner(owner)
{
    textureManager.setTexture(sprite, "robot");
    sprite.setRotation(0);
    sprite.setScale(size / 30.0, size / 30.0);
    if (owner == 0)
        sprite.setColor(sf::Color(24, 161, 212));
    else
        sprite.setColor(sf::Color(231, 24, 118));
    setPosition(position);
}
    
void Nuke::update(float delta)
{
    setPosition(getPosition() + velocity * delta);
    velocity = velocity * powf(0.3, delta);
    sprite.setRotation(sprite.getRotation() + delta * 10 * sf::length(velocity));
    if (velocity < 10.0f)
        explode();
}

void Nuke::collide(Collisionable* other, float force)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e && e->takeDamage(getPosition(), -1, 4))
        explode();
}

void Nuke::render(sf::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    window.draw(sprite);
}

void Nuke::explode()
{
    if (size > 5.0)
    {
        for(unsigned int n=0; n<10;n++)
            new Nuke(getPosition(), sf::vector2FromAngle(random(0, 360)) * size * 8.0f, size / 1.5f, owner);
    }
    new Explosion(getPosition(), size);
    destroy();
}
