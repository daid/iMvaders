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
: Collisionable(glm::vec2(16, 20))
{
    setCollisionPhysics(true, false);

    health = maxHealth;
    if (random(0, 100) < 50)
        spriteManager.setTexture(sprite, "thingomatic");
    else
        spriteManager.setTexture(sprite, "cupcake");
    setRotation(random(0, 360));
    setVelocity(glm::vec2(random(-1, 1), random(-1, 1)) * 3.0f + glm::vec2(0.0f, 25.0f));
    setPosition(glm::vec2(random(-20, 340), -20));
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

void OldPrinter::render(sp::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.setRotation(getRotation());
    sprite.draw(window);
}

void OldPrinter::collide(Collisionable* other, float force)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e)
        e->takeDamage(getPosition(), 0, 1);
}

bool OldPrinter::takeDamage(glm::vec2 position, int damageType, int damageAmount)
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
