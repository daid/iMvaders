#include <math.h>
#include "nuke.h"
#include "vectorUtils.h"
#include "soundManager.h"
#include "textureManager.h"
#include "random.h"
#include "vectorUtils.h"
#include "explosion.h"

Nuke::Nuke(glm::vec2 position, glm::vec2 velocity, float size, int owner)
: GameEntity(), Collisionable(size), velocity(velocity), size(size), owner(owner)
{
    spriteManager.setTexture(sprite, "robot");
    sprite.setRotation(0);
    sprite.setScale(size / 30.0f, size / 30.0f);
    if (owner == 0)
        sprite.setColor({24, 161, 212, 255});
    else
        sprite.setColor({231, 24, 118, 255});
    setPosition(position);
}

void Nuke::update(float delta)
{
    setPosition(getPosition() + velocity * delta);
    velocity = velocity * powf(0.3, delta);
    sprite.setRotation(sprite.getRotation() + delta * 10 * glm::length(velocity));
    if (glm::length(velocity) < 10.0f)
        explode();
}

void Nuke::collide(Collisionable* other, float force)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e && e->takeDamage(getPosition(), -1, 4))
        explode();
}

void Nuke::render(sp::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.draw(window);
}

void Nuke::explode()
{
    if (size > 5.0f)
    {
        for(unsigned int n=0; n<10;n++)
            new Nuke(getPosition(), vec2FromAngle(random(0, 360)) * size * 8.0f, size / 1.5f, owner);
    }
    new Explosion(getPosition(), size);
    destroy();
}
