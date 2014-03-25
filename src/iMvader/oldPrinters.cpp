#include "scriptInterface.h"
#include "random.h"
#include "textureManager.h"
#include "scoreManager.h"
#include "engine.h"
#include "explosion.h"
#include "gameEntity.h"
#include "Collisionable.h"

class OldPrinter : public GameEntity, public Collisionable
{
    int health;
    static const int maxHealth = 35;
    sf::Vector2f velocity;
public:
    OldPrinter()
    : Collisionable(10)
    {
        health = maxHealth;
        if (random(0, 100) < 50)
            textureManager.setTexture(sprite, "thingomatic");
        else
            textureManager.setTexture(sprite, "cupcake");
        sprite.setRotation(random(0, 360));
        velocity = sf::Vector2f(random(-1, 1), random(-1, 1)) * 3.0f + sf::Vector2f(0.0f, 25.0f);
        setPosition(sf::Vector2f(random(-30, 350), -20));
    }
    virtual ~OldPrinter() {}

    virtual void update(float delta)
    {
        setPosition(getPosition() + velocity * delta);
        if (getPosition().y > 260)
            destroy();
    }
    
    virtual void render(sf::RenderTarget& window)
    {
        sprite.setPosition(getPosition());
        window.draw(sprite);
    }

    virtual void collision(Collisionable* other)
    {
        GameEntity* e = dynamic_cast<GameEntity*>(other);
        if (e)
            e->takeDamage(getPosition(), 0, 1);
    }
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
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
};

REGISTER_SCRIPT_CLASS(OldPrinter)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, setPosition);
}
