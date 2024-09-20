#include <math.h>

#include "postProcessManager.h"
#include "player.h"
#include "textureManager.h"
#include "vectorUtils.h"
#include "bullet.h"
#include "explosion.h"
#include "nuke.h"
#include "random.h"

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
        spriteManager.setTexture(sprite, "player1");
        color = {24, 161, 212, 255};
    }
    else if (type < 0)
    {
        spriteManager.setTexture(sprite, "BasicEnemy", 0);
        color = {212, 0, 0, 255};
        sprite.setScale(1.2, 1.2);
        sprite.setColor(color);
    }
    else
    {
        spriteManager.setTexture(sprite, "player2");
        color = {231, 24, 118, 255};
    }
    
    if (type >= 0)
        setPosition({160 + random(-100, 100), 220});
    else
        setPosition({160, 20});
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
        velocity = glm::vec2(-speed, velocity.y);
    if (controller->right())
        velocity = glm::vec2( speed, velocity.y);
    if (controller->up())
        velocity = glm::vec2(velocity.x, -speed);
    if (controller->down())
        velocity = glm::vec2(velocity.x,  speed);

    setPosition(getPosition() + velocity * delta);

    if (getPosition().x < 20)
        setPosition(glm::vec2(20, getPosition().y));
    if (getPosition().x > 300)
        setPosition(glm::vec2(300, getPosition().y));
    if (getPosition().y < 10)
        setPosition(glm::vec2(getPosition().x, 10));
    if (getPosition().y > 230)
        setPosition(glm::vec2(getPosition().x, 230));
    if (getPosition().y < 20 && type >= 0)
        setPosition(glm::vec2(getPosition().x, 20));
    if (getPosition().y > 220 && type < 0)
        setPosition(glm::vec2(getPosition().x, 220));

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
                    new Bullet(getPosition(), -1 - type, (float(n) - float(shots) / 2.0f) / float(shots) * 15.0f + ((type >= 0) ? 0 : 180));
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
                new Bullet(getPosition() + glm::vec2(7, 0), -2, 0);
                new Bullet(getPosition() + glm::vec2(-4, 0), -2, 0);
                fireCooldown = 0.8;
            }
        }
        if (!controller->button(fireButton))
        {
            if (type <= 0 && fireCooldown > 0.1f)
                fireCooldown = 0.1;
            if (type == 1 && fireCooldown > 0.25f)
                fireCooldown = 0.25;
        }
        if (nukeCooldown <= 0 && controller->button(nukeButton) && info->nukes > 0 && !controller->button(chargeShotButton))
        {
            nukeCooldown = 2.0;
            info->nukes -= 1;
            new Nuke(getPosition(), glm::vec2(0.0, -150.0), 10.0, type);
        }
    }
}

void PlayerCraft::render(sp::RenderTarget& window)
{
    if (fmod(invulnerability, 4.0/60.0) > 2.0/60.0)
        return;
    sprite.setPosition(getPosition());
    if (type < 0)
        sprite.setRotation(velocity.x / 10.0f + 180);
    else
        sprite.setRotation(velocity.x / 10.0f);
    sprite.draw(window);
    
    if (chargeShot > minChargeShot)
    {
        float r = 0.5f + 3 * (chargeShot - minChargeShot) / (maxChargeShot - minChargeShot);
        CircleShape circle(r, random(3, 5));
        circle.setOrigin(r, r);
        auto col = color;
        col.a = 200;
        circle.setFillColor(col);
        circle.setRotation(random(0, 360));
        circle.setPosition(getPosition() + rotateVec2(glm::vec2(0, -1), sprite.getRotation()) * 10.0f);
        circle.draw(window);
    }
}

bool PlayerCraft::takeDamage(glm::vec2 position, int damageType, int damageAmount)
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
            new Explosion(sprite.getPosition() + glm::vec2(random(-10, 10), random(-10, 10)), 10);
        }
        //new Explosion(getPosition(), 12);
    }
    return true;
}

PlayerBonusLaser::PlayerBonusLaser(P<PlayerCraft> owner)
: Collisionable(glm::vec2(6, 240), glm::vec2(0, -120)), owner(owner)
{
    length = 240.0;
}

void PlayerBonusLaser::update(float delta)
{
    length = 240.0;
    if (damageTarget)
    {
        damageTarget->takeDamage(getPosition(), -type, 1);
    }
    if (owner)
        setPosition(owner->getPosition());
    else
        destroy();
}

void PlayerBonusLaser::render(sp::RenderTarget& window)
{
    RectangleShape laser(glm::vec2(6, length));
    laser.setOrigin(3, length);
    if (type == 1)
        laser.setFillColor({24, 161, 212,128});
    else
        laser.setFillColor({231, 24, 118,128});
    laser.setPosition(getPosition());
    laser.setRotation(getRotation());
    laser.draw(window);
}

void PlayerBonusLaser::collide(Collisionable* other, float force)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e)
    {
        if (e->takeDamage(getPosition(), -type, 0))
        {
            float dist = glm::length(other->getPosition() - getPosition());
            if (dist < length)
            {
                length = dist;
                damageTarget = e;
            }
        }
    }
}
