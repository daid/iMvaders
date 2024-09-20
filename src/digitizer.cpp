#include "digitizer.h"
#include "scoreManager.h"
#include "engine.h"
#include "random.h"
#include "vectorUtils.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(Digitizer)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, setPosition);
    //REGISTER_SCRIPT_CLASS_FUNCTION(Digitizer, setTargetPosition);
}

Digitizer::Digitizer()
: GameEntity(), Collisionable(glm::vec2(28.0f, 35.0f))
{
    spriteManager.setTexture(sprite, "Digitizer", 0);
    health = maxHealth;
    state = 0;
    setPosition(glm::vec2(random(50, 320 - 50), -50));
}

Digitizer::~Digitizer(){}

void Digitizer::update(float delta)
{
    switch(state)
    {
    case 0:
        if (getPosition().y < 50)
        {
            setPosition(glm::vec2(getPosition().x, getPosition().y + delta * 100.0f));
        }else{
            state = 1;
            laser[0] = new DigitizerLaser(this);
            laser[0]->setPosition(getPosition() + glm::vec2( 12.0, -13.0));
            laser[0]->setRotation(25.0f);
            laser[1] = new DigitizerLaser(this);
            laser[1]->setPosition(getPosition() + glm::vec2(-12.0, -13.0));
            laser[1]->setRotation(-25.0f);
        }
        break;
    case 1:
        if (!laser[0])
            state = 2;
        break;
    case 2:
        if (getPosition().y > -50)
        {
            setPosition(glm::vec2(getPosition().x, getPosition().y - delta * 150.0f));
        }else{
            setPosition(glm::vec2(random(50, 320 - 50), -50));
            state = 0;
        }
        break;
    }
}

void Digitizer::render(sp::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.draw(window);
}
void Digitizer::collide(Collisionable* target, float force)
{
    GameEntity* e = dynamic_cast<GameEntity*>(target);
    if (e)
        e->takeDamage(getPosition(), 0, 1);
}

bool Digitizer::takeDamage(glm::vec2 position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    health -= damageAmount;
    if (health <= 0)
    {
        new Explosion(getPosition(), 8);
        destroy();
        P<ScoreManager>(engine->getObject("score"))->add(40);
    }
    return true;
}

DigitizerLaser::DigitizerLaser(P<Digitizer> owner)
: Collisionable(glm::vec2(1, 240), glm::vec2(0, 120))
{
    this->owner = owner;
    activateDelay = activateDelayMax;
    deactivateDelay = deactivateDelayMax;
}

void DigitizerLaser::update(float delta)
{
    if (activateDelay >= 0.0f)
        activateDelay -= delta;
    else if (deactivateDelay >= 0.0f)
        deactivateDelay -= delta;
    else
        destroy();
    if (!owner)
        destroy();
}

void DigitizerLaser::render(sp::RenderTarget& window)
{
    if (activateDelay < 0)
    {
        RectangleShape laser(glm::vec2(3, 240));
        laser.setOrigin(1.5, 0);
        laser.setFillColor({255,0,0,192});
        laser.setRotation(getRotation());
        laser.setPosition(getPosition());
        laser.draw(window);
    }
    else
    {
        RectangleShape laser(glm::vec2(1, 240));
        laser.setOrigin(0.5, 0);
        laser.setFillColor({255,0,0,64 + 64 * ((activateDelayMax - activateDelay) / activateDelayMax)});
        laser.setRotation(getRotation());
        laser.setPosition(getPosition());
        laser.draw(window);
    }
}

void DigitizerLaser::collide(Collisionable* other, float force)
{
    if (activateDelay < 0)
    {
        GameEntity* e = dynamic_cast<GameEntity*>(other);
        if (e)
            e->takeDamage(getPosition(), 1, 1);
    }
}
