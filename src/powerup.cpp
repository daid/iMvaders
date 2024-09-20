#include <math.h>

#include "powerup.h"
#include "textureManager.h"
#include "player.h"

Powerup::Powerup()
: GameEntity(), Collisionable(8.0)
{
    speed = 40;
    spriteManager.setTexture(sprite, "robot", 0);
    sprite.setScale(0.2, 0.2);
}

Powerup::~Powerup()
{
}

void Powerup::update(float delta)
{
    setPosition(getPosition() + glm::vec2(0, speed) * delta);
    if (getPosition().y > 240 + 20)
        destroy();
}

void Powerup::collide(Collisionable* other, float force)
{
    P<PlayerCraft> craft = dynamic_cast<PlayerCraft*>(other);
    if (!craft)
        return;
    craft->info->nukes++;
    destroy();
}

void Powerup::render(sp::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.draw(window);
}
