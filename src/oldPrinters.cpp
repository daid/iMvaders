#include "oldPrinters.h"
#include "scriptInterface.h"
#include "random.h"
#include "textureManager.h"
#include "scoreManager.h"
#include "engine.h"
#include "explosion.h"
#include "engine.h"

REGISTER_SCRIPT_CLASS(OldPrinter)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, setPosition);
}

OldPrinter::OldPrinter()
: Collisionable(sf::Vector2f(16, 20))
{
    setCollisionPhysics(true, false);

    health = maxHealth;
    if (random(0, 100) < 50)
        textureManager.setTexture(sprite, "thingomatic");
    else
        textureManager.setTexture(sprite, "cupcake");
    setRotation(random(0, 360));
    setVelocity(sf::Vector2f(random(-1, 1), random(-1, 1)) * 3.0f + sf::Vector2f(0.0f, 25.0f));
    setPosition(sf::Vector2f(random(-20, 340), -20));
}

void OldPrinter::update(float delta)
{
    if (getPosition().y > 260)
        destroy();
    if (getPosition().x < -30)
        destroy();
    if (getPosition().x > 350)
        destroy();
}

void OldPrinter::render(sf::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.setRotation(getRotation());
    window.draw(sprite);
}

void OldPrinter::collision(Collisionable* other)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e)
        e->takeDamage(getPosition(), 0, 1);
}

bool OldPrinter::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    health -= damageAmount;
    if (health <= 0)
    {
        new Explosion(getPosition(), 8);
        destroy();
        P<ScoreManager>(engine->getObject("score"))->add(25);
    }
    return true;
}
