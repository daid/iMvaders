#include <math.h>

#include "random.h"
#include "bre.h"
#include "bullet.h"
#include "textureManager.h"
#include "scoreManager.h"
#include "explosion.h"

BreEnemy::BreEnemy()
: GameEntity(), Collisionable(50.0)
{
    textureManager.setTexture(sprite, "bre1");
    sprite.setPosition(sf::Vector2f(160, -80));
    textureManager.setTexture(mouth, "bre2");
    mouth.setPosition(sf::Vector2f(160, -80));
    state = BS_FlyIn;
    mouthPos = 0;
    shotDelay = 0;
    invulnerability = 0;
    moveSpeed = 60;
    health = maxHealth;
    //setCollisionRadius(50.f);
}

BreEnemy::~BreEnemy()
{
}

void BreEnemy::update(float delta)
{
    if (invulnerability > 0)
        invulnerability -= delta;
    switch(state)
    {
    case BS_FlyIn:
        if (sprite.getPosition().y < 80.0)
        {
            sprite.setPosition(sprite.getPosition() + sf::Vector2f(0, 1));
        }else
        {
            state = BS_MoveLeftRight;
            moveDir = random(0, 100) < 50 ? 1 : -1;
        }
        break;
    case BS_MoveLeftRight:
        sprite.setPosition(sprite.getPosition() + sf::Vector2f(moveDir * moveSpeed * delta, 0));
        if (sprite.getPosition().x < 40.0)
            moveDir = 1;
        if (sprite.getPosition().x > 280.0)
            moveDir = -1;

        if (shotDelay > 0)
        {
            shotDelay -= delta;
        }else{
            if (random(0, 100) < 30)
            {
                if (random(0, 100) < 50)
                {
                    state = BS_LaserCharge;
                    shotDelay = laserChargeTime;
                }else{
                    state = BS_MouthOpen;
                    enemySpawnCount = 5;
                }
            }else{
                for(int n=-2; n<=2; n++)
                    new Bullet(sprite.getPosition() + sf::Vector2f(-n*15, -50), 0, 180 + n * 7, 90.0);
                shotDelay = normalShotDelay;
            }
        }
        break;
    case BS_LaserCharge:
        if (shotDelay > 0)
        {
            shotDelay -= delta;
        }else{
            state = BS_LaserFire;
            shotDelay = 0.5;
        }
        break;
    case BS_LaserFire:
        if (shotDelay > -0.5)
        {
            shotDelay -= delta;
        }
        else
        {
            shotDelay = normalShotDelay;
            state = BS_MoveLeftRight;
        }
        break;
    case BS_MouthOpen:
        if (mouthPos < 30)
        {
            mouthPos += delta * 40;
            shotDelay = 0.2;
        }else if (shotDelay > 0)
        {
            shotDelay -= delta;
        }else{
            shotDelay = 0.2;
            if (enemySpawnCount > 0)
            {
                BasicEnemyBase* e = new BasicEnemy();
                e->setTargetPosition(sf::Vector2f(random(20, 300), random(20, 200)));
                e->wait(sprite.getPosition() + sf::Vector2f(0, 50), sprite.getPosition() + sf::Vector2f(0, 100));
                e->flyIn();
                enemyList.push_back(e);
                enemySpawnCount --;
            }else{
                state = BS_MouthClose;
            }
        }
        break;
    case BS_MouthClose:
        if (mouthPos > 0)
        {
            mouthPos -= delta * 40;
        }
        else
        {
            mouthPos = 0;
            shotDelay = normalShotDelay;
            state = BS_MoveLeftRight;
        }
    }
    mouth.setPosition(sprite.getPosition() + sf::Vector2f(0, mouthPos));
    setPosition(sprite.getPosition()); // Set position for collision
    foreach(BasicEnemyBase, e, enemyList)
    {
        if (e->state == ES_CenterField)
            e->dive(sf::Vector2f(random(20, 300), 340));
        if (e->state == ES_Outside)
        {
            e->wait(sf::Vector2f(random(20, 300), -40), sprite.getPosition() + sf::Vector2f(0, 100));
            e->flyIn();
        }
    }
}

void BreEnemy::render(sf::RenderTarget& window)
{
    if ((fmodf(invulnerability, 4.0/60) > 2.0/60.0) || (state == BS_LaserCharge && (fmodf(shotDelay, 2.0/60.0) > 1.0/60.0)))
    {
        sprite.setColor(sf::Color(212, 0, 0));
        mouth.setColor(sf::Color(212, 0, 0));
    }
    else
    {
        sprite.setColor(sf::Color::White);
        mouth.setColor(sf::Color::White);
    }
    window.draw(sprite);
    window.draw(mouth);

    if (state == BS_LaserFire)
    {
        sf::RectangleShape laser(sf::Vector2f(6, 240));
        laser.setOrigin(3, 0);
        laser.setFillColor(sf::Color(255,0,0,192));
        laser.setRotation(-shotDelay*30.0);
        laser.setPosition(sprite.getPosition() + sf::Vector2f(18, 7));
        window.draw(laser);

        laser.setRotation(shotDelay*30.0);
        laser.setPosition(sprite.getPosition() + sf::Vector2f(-18, 7));
        window.draw(laser);
    }
}

void BreEnemy::postRender(sf::RenderTarget& window)
{
    sf::RectangleShape healthBarBG(sf::Vector2f(280, 10));
    healthBarBG.setFillColor(sf::Color::Transparent);
    healthBarBG.setOutlineColor(sf::Color(128, 128, 128, 128));
    healthBarBG.setOutlineThickness(1);
    healthBarBG.setPosition(20, 10);
    window.draw(healthBarBG);

    sf::RectangleShape healthBar(sf::Vector2f(280 * health / maxHealth, 10));
    healthBar.setFillColor(sf::Color(212, 0, 0, 128));
    healthBar.setPosition(20, 10);
    window.draw(healthBar);
}

bool BreEnemy::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    if (invulnerability > 0)
        return true;

    if (state != BS_FlyIn)
        health -= damageAmount;

    if (health <= 0)
    {
        health = 0;
        destroy();
        score.add(500);
        for(unsigned int n=0; n<20; n++)
        {
            new Explosion(sprite.getPosition() + sf::Vector2f(random(-50, 50), random(-80, 80)), 10);
        }
    }
    invulnerability = 0.25;
    return true;
}
