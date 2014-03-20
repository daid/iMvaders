#include "digitizer.h"
#include "scoreManager.h"
#include "engine.h"
#include "vectorUtils.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(Digitizer)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, setPosition);
    //REGISTER_SCRIPT_CLASS_FUNCTION(Digitizer, setTargetPosition);
}

Digitizer::Digitizer()
: GameEntity(), Collisionable(15.0f)
{
    textureManager.setTexture(sprite, "Digitizer", 0);
    health = maxHealth;
    state = 0;
    setPosition(sf::Vector2f(random(50, 320 - 50), -50));
}

Digitizer::~Digitizer(){}

void Digitizer::update(float delta)
{
    switch(state)
    {
    case 0:
        if (getPosition().y < 50)
        {
            setPosition(sf::Vector2f(getPosition().x, getPosition().y + delta * 100.0f));
        }else{
            state = 1;
            laser[0] = new DigitizerLaser(this);
            laser[0]->setPosition(getPosition() + sf::Vector2f( 12.0, -13.0));
            laser[0]->setCollisionLineVector(sf::vector2FromAngle( 25.0f) * -240.0f);
            laser[1] = new DigitizerLaser(this);
            laser[1]->setPosition(getPosition() + sf::Vector2f(-12.0, -13.0));
            laser[1]->setCollisionLineVector(sf::vector2FromAngle(-25.0f) * -240.0f);
        }
        break;
    case 1:
        if (!laser[0])
            state = 2;
        break;
    case 2:
        if (getPosition().y > -50)
        {
            setPosition(sf::Vector2f(getPosition().x, getPosition().y - delta * 150.0f));
        }else{
            setPosition(sf::Vector2f(random(50, 320 - 50), -50));
            state = 0;
        }
        break;
    }
}

void Digitizer::render(sf::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    window.draw(sprite);
}
void Digitizer::collision(Collisionable* target)
{
    GameEntity* e = dynamic_cast<GameEntity*>(target);
    if (e)
        e->takeDamage(getPosition(), 0, 1);
}

bool Digitizer::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    health -= damageAmount;
    if (health <= 0)
    {
        new Explosion(getPosition(), 8);
        destroy();
        P<ScoreManager>(engine->getObject("score"))->add(30);
    }
    return true;
}

DigitizerLaser::DigitizerLaser(P<Digitizer> owner)
: Collisionable(sf::Vector2f(0, 240))
{
    this->owner = owner;
    activateDelay = activateDelayMax;
    deactivateDelay = deactivateDelayMax;
}

void DigitizerLaser::update(float delta)
{
    if (activateDelay >= 0.0)
        activateDelay -= delta;
    else if (deactivateDelay >= 0.0)
        deactivateDelay -= delta;
    else
        destroy();
    if (!owner)
        destroy();
}

void DigitizerLaser::render(sf::RenderTarget& window)
{
    if (activateDelay < 0)
    {
        sf::RectangleShape laser(sf::Vector2f(3, 240));
        laser.setOrigin(1.5, 240);
        laser.setFillColor(sf::Color(255,0,0,192));
        laser.setRotation(sf::vector2ToAngle(getCollisionLineVector()));
        laser.setPosition(getPosition());
        window.draw(laser);
    }
    else
    {
        sf::RectangleShape laser(sf::Vector2f(1, 240));
        laser.setOrigin(0.5, 240);
        laser.setFillColor(sf::Color(255,0,0,64 + 64 * ((activateDelayMax - activateDelay) / activateDelayMax)));
        laser.setRotation(sf::vector2ToAngle(getCollisionLineVector()));
        laser.setPosition(getPosition());
        window.draw(laser);
    }
}

void DigitizerLaser::collision(Collisionable* other)
{
    if (activateDelay < 0)
    {
        GameEntity* e = dynamic_cast<GameEntity*>(other);
        if (e)
            e->takeDamage(getPosition(), 1, 1);
    }
}

