#include <math.h>

#include "postProcessManager.h"
#include "player.h"
#include "textureManager.h"
#include "vectorUtils.h"
#include "bullet.h"
#include "explosion.h"
#include "nuke.h"

bool playerBonusWeaponsActive = false;

PlayerCraft::PlayerCraft(PlayerController* controller, PlayerInfo* info, int type)
: GameEntity(), Collisionable(10.0), controller(controller), info(info), type(type)
{
    invulnerability = 1.0;
    fireCooldown = 0.4;
    nukeCooldown = 2.0;
    chargeShot = 0;
    health = 2;
    if (type == 0)
    {
        textureManager.setTexture(sprite, "player1");
        color = sf::Color(24, 161, 212, 255);
    }
    else if (type < 0)
    {
        textureManager.setTexture(sprite, "BasicEnemy", 0);
        color = sf::Color(212, 0, 0, 255);
        sprite.setScale(1.2, 1.2);
        sprite.setColor(color);
    }
    else
    {
        textureManager.setTexture(sprite, "player2");
        color = sf::Color(231, 24, 118, 255);
    }
    
    if (type >= 0)
        setPosition(sf::Vector2f(160, 220));
    else
        setPosition(sf::Vector2f(160, 20));
}

PlayerCraft::~PlayerCraft()
{
}

void PlayerCraft::update(float delta)
{
    if (fireCooldown > 0)
        fireCooldown -= delta;
    if (nukeCooldown > 0)
        nukeCooldown -= delta;
    if (invulnerability > 0)
        invulnerability -= delta;

    velocity = velocity * 0.85f;//TODO: Proper dampening with use of delta.
    float speed = 100;
    if (controller->button(slowButton))
        speed = 40;
    if (controller->left())
        velocity = sf::Vector2f(-speed, velocity.y);
    if (controller->right())
        velocity = sf::Vector2f( speed, velocity.y);
    if (controller->up())
        velocity = sf::Vector2f(velocity.x, -speed);
    if (controller->down())
        velocity = sf::Vector2f(velocity.x,  speed);

    setPosition(getPosition() + velocity * delta);

    if (getPosition().x < 20)
        setPosition(sf::Vector2f(20, getPosition().y));
    if (getPosition().x > 300)
        setPosition(sf::Vector2f(300, getPosition().y));
    if (getPosition().y < 10)
        setPosition(sf::Vector2f(getPosition().x, 10));
    if (getPosition().y > 230)
        setPosition(sf::Vector2f(getPosition().x, 230));
    if (getPosition().y < 20 && type >= 0)
        setPosition(sf::Vector2f(getPosition().x, 20));
    if (getPosition().y > 220 && type < 0)
        setPosition(sf::Vector2f(getPosition().x, 220));

    if (playerBonusWeaponsActive)
    {
        if (controller->button(fireButton) || controller->button(chargeShotButton))
        {
            if (!bonusLaser)
                bonusLaser = new PlayerBonusLaser(this);
            if (controller->button(fireButton))
                bonusLaser->type = type ? 2 : 1;
            else
                bonusLaser->type = type ? 1 : 2;
        }else{
            if (bonusLaser)
                bonusLaser->destroy();
        }
    }else{
        if (bonusLaser)
            bonusLaser->destroy();
        
        if (controller->button(chargeShotButton))
        {
            chargeShot += delta;
            if (chargeShot > maxChargeShot) chargeShot = maxChargeShot;
        }else if (chargeShot > 0)
        {
            int shots = 5 * (chargeShot - minChargeShot) / (maxChargeShot - minChargeShot);
            chargeShot = 0.0;
            if (shots > 0)
            {
                for(int n=0; n<=shots; n++)
                {
                    new Bullet(getPosition(), -1 - type, (float(n) - float(shots) / 2.0) / float(shots) * 15.0 + ((type >= 0) ? 0 : 180));
                }
            }
        }
        
        if (controller->button(fireButton) && fireCooldown <= 0 && invulnerability <= 0 && !controller->button(chargeShotButton))
        {
            if (type == 0)
            {
                new Bullet(getPosition(), -1, 0);
                fireCooldown = 0.4;
            }
            if (type == -1)
            {
                new Bullet(getPosition(), 0, 180);
                fireCooldown = 0.4;
            }
            if (type == 1)
            {
                new Bullet(getPosition() + sf::Vector2f(7, 0), -2, 0);
                new Bullet(getPosition() + sf::Vector2f(-4, 0), -2, 0);
                fireCooldown = 0.8;
            }
        }
        if (!controller->button(fireButton))
        {
            if (type <= 0 && fireCooldown > 0.1)
                fireCooldown = 0.1;
            if (type == 1 && fireCooldown > 0.25)
                fireCooldown = 0.25;
        }
        if (nukeCooldown <= 0 && controller->button(nukeButton) && info->nukes > 0 && !controller->button(chargeShotButton))
        {
            nukeCooldown = 2.0;
            info->nukes -= 1;
            new Nuke(getPosition(), sf::Vector2f(0.0, -150.0), 10.0, type);
        }
    }
}

void PlayerCraft::render(sf::RenderTarget& window)
{
    if (fmod(invulnerability, 4.0/60.0) > 2.0/60.0)
        return;
    sprite.setPosition(getPosition());
    if (type < 0)
        sprite.setRotation(velocity.x / 10.0 + 180);
    else
        sprite.setRotation(velocity.x / 10.0);
    window.draw(sprite);
    
    if (chargeShot > minChargeShot)
    {
        float r = 0.5 + 3 * (chargeShot - minChargeShot) / (maxChargeShot - minChargeShot);
        sf::CircleShape circle(r, random(3, 5));
        circle.setOrigin(r, r);
        sf::Color col = color;
        col.a = 200;
        circle.setFillColor(col);
        circle.setRotation(random(0, 360));
        circle.setPosition(getPosition() + sf::vector2FromAngle(sprite.getRotation()) * 10.0f);
        window.draw(circle);
    }
}

bool PlayerCraft::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (type < 0)
    {
        if (damageType >= 0 || invulnerability > 0)
            return false;
    }else{
        if (damageType < 0 || invulnerability > 0)
            return false;
    }
    health -= damageAmount;
    invulnerability = 1.0;
    if(health <= 0)
    {
        destroy();
        for(unsigned int n=0; n<4; n++)
        {
            new Explosion(sprite.getPosition() + sf::Vector2f(random(-10, 10), random(-10, 10)), 10);
        }
        //new Explosion(getPosition(), 12);
    }
    return true;
}

PlayerBonusLaser::PlayerBonusLaser(P<PlayerCraft> owner)
: Collisionable(sf::Vector2f(0, -240)), owner(owner)
{
}

void PlayerBonusLaser::update(float delta)
{
    if (owner)
        setPosition(owner->getPosition());
    else
        destroy();
}
void PlayerBonusLaser::preRender(sf::RenderTarget& window)
{
    sf::RectangleShape laser(sf::Vector2f(6, 240));
    laser.setOrigin(3, 240);
    if (type == 1)
        laser.setFillColor(sf::Color(24, 161, 212,128));
    else
        laser.setFillColor(sf::Color(231, 24, 118,128));
    laser.setPosition(getPosition());
    window.draw(laser);
}

void PlayerBonusLaser::collision(Collisionable* other)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e)
        e->takeDamage(getPosition(), -type, 0);
}
