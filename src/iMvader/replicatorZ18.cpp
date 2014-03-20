#include "replicatorZ18.h"
#include "scoreManager.h"
#include "engine.h"
#include "explosion.h"
#include "vectorUtils.h"
#include "textureManager.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(ReplicatorZ18)
{
}

ReplicatorZ18::ReplicatorZ18()
{
    for(unsigned int y=0; y<34; y++)
    {
        for(unsigned int x=0; x<20; x++)
        {
            P<ReplicatorZ18Part> part = new ReplicatorZ18Part(this, x + y * 20);
            part->setPosition(sf::Vector2f(x * 16, float(y) * 15.5 - 34.0 * 16.0));
            parts.push_back(part);
        }
    }
}

ReplicatorZ18Part::ReplicatorZ18Part(P<ReplicatorZ18> owner, int index)
: Collisionable(7)
{
    textureManager.setTexture(sprite, "Replicator_Z18", index);
    health = maxHealth;
}

void ReplicatorZ18Part::update(float delta)
{
    setPosition(getPosition() + sf::Vector2f(0, 10) * delta);
    if (getPosition().y > 260)
        destroy();
}

void ReplicatorZ18Part::render(sf::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    window.draw(sprite);
}

void ReplicatorZ18Part::collision(Collisionable* other)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e)
        e->takeDamage(getPosition(), 0, 1);
}

bool ReplicatorZ18Part::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    health -= damageAmount;
    if (health <= 0)
    {
        new Explosion(getPosition(), 8);
        destroy();
        P<ScoreManager>(engine->getObject("score"))->add(5);
    }
    return true;
}
