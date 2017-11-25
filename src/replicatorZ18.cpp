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
    y = 0;
    row_done = 34;
    speed = 16;
}

void ReplicatorZ18::update(float delta)
{
    y += speed * delta;
    int want_row = std::max(0, 34 - int(y / 16.0));
    for(int row = want_row; row < row_done; row++)
    {
        for(unsigned int x=0; x<20; x++)
        {
            P<ReplicatorZ18Part> part = new ReplicatorZ18Part(this, x + row * 20);
            part->setPosition(sf::Vector2f(x * 16, float(row) * 15 - 34.0 * 16.0 + y));
            part->setSpeed(speed);
            parts.push_back(part);
        }
        for(unsigned int x=0; x<20; x++)
        {
            int idx = parts.size() - 20 + x;
            if (x > 0) parts[idx]->neighbours[0] = parts[idx - 1];
            if (x < 19) parts[idx]->neighbours[1] = parts[idx + 1];
        }
    }
    row_done = want_row;

    if (y > 34 * 16.0 + 260)
        destroy();
}

void ReplicatorZ18::setSpeed(float speed)
{
    this->speed = speed;
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
    indestructible = false;
    
    setCollisionFilter(0x0002, 0xFFFD);
}

void ReplicatorZ18Part::destroy()
{
    GameEntity::destroy();
    for(unsigned int n=0; n<4; n++)
        neighbours[n] = NULL;
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
    {
        destroy();
    }
}

void ReplicatorZ18Part::render(sf::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.setRotation(getRotation());
    if (health <= 0)
        sprite.setColor(sf::Color(255,255,255,128));
    if (indestructible)
        sprite.setColor(sf::Color(192,192,192,255));
    window.draw(sprite);
}

void ReplicatorZ18Part::collide(Collisionable* other, float force)
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
    if (indestructible)
        return true;
    health -= damageAmount;
    if (health <= 0)
    {
        new Explosion(getPosition(), 8);
        //destroy();
        P<ScoreManager>(engine->getObject("score"))->add(1);
        if (damageType > -100)
        {
            for(unsigned int n=0; n<4; n++)
            {
                if (neighbours[n])
                    neighbours[n]->takeDamage(position, -100, 10);
            }
        }
    }
    return true;
}

void ReplicatorZ18Part::setSpeed(float speed)
{
    this->speed = speed;
}
