#include "bullet.h"
#include "vectorUtils.h"
#include "soundManager.h"
#include "textureManager.h"
#include "random.h"
#include "explosion.h"

Bullet::Bullet(sf::Vector2f position, int type, float angle, float speed)
: GameEntity(), Collisionable(1.0), speed(speed), type(type)
{
    soundManager->playSound("laser", random(0.75, 1.25));
    textureManager.setTexture(sprite, "bullet");
    setPosition(position);
    setRotation(angle);
    if (type != -1)
        setVelocity(sf::rotateVector(sf::Vector2f(0, -1), getRotation()) * speed);
    if (type == 0)
        sprite.setColor(sf::Color::Red);
    else if (type == 1)
        sprite.setColor(sf::Color::Green);
    else if (type == -1)
        sprite.setColor(sf::Color(24, 161, 212));
    else if (type == -2)
        sprite.setColor(sf::Color(231, 24, 118));
}

void Bullet::update(float delta)
{
    if (type == -1)
        setPosition(getPosition() + sf::rotateVector(sf::Vector2f(0, -1), getRotation()) * speed * delta);
    if (getPosition().x < -10) destroy();
    if (getPosition().y < -10) destroy();
    if (getPosition().x > 330) destroy();
    if (getPosition().y > 250) destroy();
}

void Bullet::collide(Collisionable* other)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e && e->takeDamage(getPosition(), type, 1))
    {
        destroy();
        new Explosion(getPosition(), 3, sf::rotateVector(sf::Vector2f(0, -1), getRotation()) * speed / 5.0f);
    }
}

void Bullet::render(sf::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.setRotation(getRotation());
    window.draw(sprite);
}
