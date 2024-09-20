#include "bullet.h"
#include "vectorUtils.h"
#include "soundManager.h"
#include "textureManager.h"
#include "random.h"
#include "explosion.h"

Bullet::Bullet(glm::vec2 position, int type, float angle, float speed)
: GameEntity(), Collisionable(1.0), speed(speed), type(type)
{
    soundManager->playSound("laser.wav", random(0.75, 1.25));
    spriteManager.setTexture(sprite, "bullet");
    setPosition(position);
    setRotation(angle);
    if (type != -1)
        setVelocity(rotateVec2(glm::vec2(0, -1), getRotation()) * speed);
    if (type == 0)
        sprite.setColor({255,0,0,255});
    else if (type == 1)
        sprite.setColor({0,255,0,255});
    else if (type == -1)
        sprite.setColor({24, 161, 212, 255});
    else if (type == -2)
        sprite.setColor({231, 24, 118, 255});
}

void Bullet::update(float delta)
{
    if (type == -1)
        setPosition(getPosition() + rotateVec2(glm::vec2(0, -1), getRotation()) * speed * delta);
    if (getPosition().x < -10) destroy();
    if (getPosition().y < -10) destroy();
    if (getPosition().x > 330) destroy();
    if (getPosition().y > 250) destroy();
}

void Bullet::collide(Collisionable* other, float force)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e && e->takeDamage(getPosition(), type, 1))
    {
        destroy();
        new Explosion(getPosition(), 3, rotateVec2(glm::vec2(0, -1), getRotation()) * speed / 5.0f);
    }
}

void Bullet::render(sp::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.setRotation(getRotation());
    sprite.draw(window);
}
