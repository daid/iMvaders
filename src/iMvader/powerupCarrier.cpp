#include <math.h>

#include "powerup.h"
#include "powerupCarrier.h"
#include "explosion.h"
#include "textureManager.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(PowerupCarrier)
{
    REGISTER_SCRIPT_CLASS_CALLBACK(PowerupCarrier, destroyed);
}

PowerupCarrier::PowerupCarrier()
: GameEntity(), Collisionable(12.0)
{
    speed = 40;
    textureManager.setTexture(sprite, "replicator2", 0);
    setPosition(sf::Vector2f(-20, 0));
}

PowerupCarrier::~PowerupCarrier()
{
}

void PowerupCarrier::update(float delta)
{
    setPosition(sf::Vector2f(getPosition().x + delta * speed, sin(getPosition().x / 30) * 30 + 50));
    if (getPosition().x > 320 + 20)
    {
        destroyed();
        destroy();
    }
}

bool PowerupCarrier::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    destroyed();
    destroy();
    new Explosion(getPosition(), 12);
    P<Powerup> powerup = new Powerup();
    powerup->setPosition(getPosition());
    return true;
}

void PowerupCarrier::render(sf::RenderTarget& window)
{
    sf::Sprite powerupSprite;
    textureManager.setTexture(powerupSprite, "robot", 0);
    powerupSprite.setPosition(getPosition());
    powerupSprite.setScale(0.1, 0.1);
    window.draw(powerupSprite);

    sprite.setPosition(getPosition());
    window.draw(sprite);
}
