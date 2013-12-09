#include "bullet.h"
#include "vectorUtils.h"
#include "soundManager.h"
#include "textureManager.h"
#include "random.h"
#include "explosion.h"

Bullet::Bullet(sf::Vector2f position, int type, float angle, float speed)
: GameEntity(1.0f), speed(speed), type(type)
{
    soundManager.setSound(sound, "laser");
    sound.setPitch(random(0.8, 1.2));
    sound.play();
    textureManager.setTexture(sprite, "bullet");
    sprite.setPosition(position);
    sprite.setRotation(angle);
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
    sprite.setPosition(sprite.getPosition() + sf::vector2FromAngle(sprite.getRotation()) * speed * delta);
    if (sprite.getPosition().x < -10) destroy();
    if (sprite.getPosition().y < -10) destroy();
    if (sprite.getPosition().x > 330) destroy();
    if (sprite.getPosition().y > 250) destroy();

    foreach_hit(e, this)
    {
        if (e->takeDamage(sprite.getPosition(), type, 1))
        {
            destroy();
            new Explosion(sprite.getPosition(), 3);
        }
    }
}

void Bullet::render(sf::RenderTarget& window)
{
    window.draw(sprite);
}
