#include "replicatorZ18.h"
#include "scoreManager.h"
#include "engine.h"
#include "explosion.h"
#include "vectorUtils.h"
#include "textureManager.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(ReplicatorZ18)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(ReplicatorZ18, setSpeed);
}

ReplicatorZ18::ReplicatorZ18()
{
    for(unsigned int y=0; y<34; y++)
    {
        for(unsigned int x=0; x<20; x++)
        {
            P<ReplicatorZ18Part> part = new ReplicatorZ18Part(this, x + y * 20);
            part->setPosition(sf::Vector2f(x * 16, float(y) * 15 - 34.0 * 16.0));
            parts.push_back(part);
        }
    }
}

void ReplicatorZ18::update(float delta)
{
    int cnt = 0;
    foreach(ReplicatorZ18Part, p, parts)
        cnt++;
    if (cnt == 0)
        destroy();
}

void ReplicatorZ18::setSpeed(float speed)
{
    foreach(ReplicatorZ18Part, p, parts)
        p->setSpeed(speed);
}

ReplicatorZ18Part::ReplicatorZ18Part(P<ReplicatorZ18> owner, int index)
: Collisionable(sf::Vector2f(16, 16))
{
    speed = 16;
    textureManager.setTexture(sprite, "Replicator_Z18", index);
    health = maxHealth;
    tinyExplosionDelay = 0.0;
}

void ReplicatorZ18Part::update(float delta)
{
    setPosition(getPosition() + sf::Vector2f(0, speed) * delta);
    if (health <= 0)
    {
        setRotation(getRotation() + 1000 * delta);
        speed += delta * 300;
        
        if (tinyExplosionDelay > 0)
        {
            tinyExplosionDelay -= delta;
        }else{
            tinyExplosionDelay += 0.1;
            new Explosion(getPosition() + sf::Vector2f(random(-6, 6), random(-6, 6)), 2, sf::Vector2f(0, speed));
        }
    }
    if (getPosition().y > 260)
        destroy();
}

void ReplicatorZ18Part::render(sf::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.setRotation(getRotation());
    if (health <= 0)
        sprite.setColor(sf::Color(255,255,255,128));
    window.draw(sprite);
}

void ReplicatorZ18Part::collision(Collisionable* other)
{
    if (health <= 0)
        return;
    
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e && e->takeDamage(getPosition(), 0, 1))
    {
        health = 0;
    }
}

bool ReplicatorZ18Part::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType >= 0 || health <= 0)
        return false;
    health -= damageAmount;
    if (health <= 0)
    {
        new Explosion(getPosition(), 8);
        //destroy();
        P<ScoreManager>(engine->getObject("score"))->add(5);
    }
    return true;
}

void ReplicatorZ18Part::setSpeed(float speed)
{
    this->speed = speed;
}
