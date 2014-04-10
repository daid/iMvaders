#include <math.h>

#include "random.h"
#include "bre.h"
#include "bullet.h"
#include "textureManager.h"
#include "scoreManager.h"
#include "explosion.h"
#include "transmission.h"
#include "engine.h"

REGISTER_SCRIPT_CLASS(BreEnemy)
{
    REGISTER_SCRIPT_CLASS_CALLBACK(BreEnemy, destroyed);
    REGISTER_SCRIPT_CLASS_FUNCTION(BreEnemy, setDifficulty);
}

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
    moneyshieldDeployed = false;
    health = maxHealth;
    difficulty = 1;
}

BreEnemy::~BreEnemy()
{
}

void BreEnemy::setDifficulty(int difficulty)
{
    this->difficulty = difficulty;
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
            sprite.setPosition(sprite.getPosition() + sf::Vector2f(0, moveSpeed * delta));
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
            if (difficulty > 1)
                shotDelay -= delta * 0.2;
        }else{
            if (random(0, 100) < 30)
            {
                if (random(0, 100) < 50)
                {
                    state = BS_LaserCharge;
                    shotDelay = laserChargeTime;
                }else{
                    state = BS_MouthOpen;
                    enemySpawnCount = 5 * difficulty;
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
            if (difficulty > 1)
                shotDelay -= delta * 0.3;
        }else{
            state = BS_LaserFire;
            laser[0] = new BreLaser(this);
            laser[1] = new BreLaser(this);
            shotDelay = 0.5;
        }
        break;
    case BS_LaserFire:
        if (shotDelay > -0.5) //Why -0.5?
        {
            shotDelay -= delta;
        }
        else
        {
            shotDelay = normalShotDelay;
            state = BS_MoveLeftRight;
            laser[0]->destroy();
            laser[1]->destroy();
        }
        break;
    case BS_MouthOpen:
        if (mouthPos < 30)
        {
            mouthPos += delta * 40;
            if (difficulty > 1)
                mouthPos += delta * 10;
            shotDelay = 0.2;
        }else if (shotDelay > 0)
        {
            shotDelay -= delta;
            if (difficulty > 1)
                shotDelay -= delta * 0.5;
        }else{
            shotDelay += 0.2;
            if (enemySpawnCount > 0)
            {
                BasicEnemyBase* e = new BasicEnemy();
                e->setTargetPosition(sf::Vector2f(random(20, 300), random(20, 200)));
                e->wait(sprite.getPosition() + sf::Vector2f(0, 50), sprite.getPosition() + sf::Vector2f(0, 100));
                e->flyIn();
                if (enemySpawnCount % 5 == 0)
                    e->giveShield();
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
    if (laser[0])
    {
        laser[0]->setPosition(getPosition() + sf::Vector2f(18, 7));
        laser[0]->setCollisionLineVector(sf::vector2FromAngle(-shotDelay*30.0f) * -240.0f);
    }
    if (laser[1])
    {
        laser[1]->setPosition(getPosition() + sf::Vector2f(-18, 7));
        laser[1]->setCollisionLineVector(sf::vector2FromAngle(shotDelay*30.0f) * -240.0f);
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
    if (fmodf(invulnerability, 4.0/60) > 2.0/60.0)
    {
        sprite.setColor(sf::Color(212, 0, 0));
        mouth.setColor(sf::Color(212, 0, 0));
    }
    else if (state == BS_LaserCharge)
    {
        float f = (shotDelay / laserChargeTime);
        sprite.setColor(sf::Color(212, 128 * f, 128 * f));
        mouth.setColor(sf::Color(212, 128 * f, 128 * f));
    }
    else
    {
        sprite.setColor(sf::Color::White);
        mouth.setColor(sf::Color::White);
    }
    window.draw(sprite);
    window.draw(mouth);
}

void BreEnemy::postRender(sf::RenderTarget& window)
{
    if (state == BS_FlyIn)
        return;
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
    if (health < maxHealth / 2 && !moneyshieldDeployed)
    {
        moneyshieldDeployed = true;
        P<Transmission> t = new Transmission();
        t->setText("Deploying corperate|money shield");
        t->top();
        
        if (difficulty < 2)
        {
            for(float f=0; f<=360; f+=20)
            {
                new MoneyShield(this, f, 100, false);
                new MoneyShield(this, f, 80, true);
            }
        }else{
            for(float f=0; f<=360; f+=20)
            {
                new MoneyShield(this, f, 110, false);
                new MoneyShield(this, f, 90, true);
                new MoneyShield(this, f, 70, false);
            }
        }
    }

    if (health <= 0)
    {
        health = 0;
        destroy();
        destroyed();
        foreach(BasicEnemyBase, e, enemyList)
            e->destroy();
        P<ScoreManager>(engine->getObject("score"))->add(500);
        if (difficulty > 1)
            P<ScoreManager>(engine->getObject("score"))->add(200);
        for(unsigned int n=0; n<20; n++)
        {
            new Explosion(sprite.getPosition() + sf::Vector2f(random(-50, 50), random(-80, 80)), 10);
        }
    }
    invulnerability = 0.10;
    return true;
}

BreLaser::BreLaser(P<BreEnemy> owner)
: Collisionable(sf::Vector2f(0, 240))
{
    this->owner = owner;
}

void BreLaser::update(float delta)
{
    if (!owner)
        destroy();
}

void BreLaser::render(sf::RenderTarget& window)
{
    sf::RectangleShape laser(sf::Vector2f(6, 240));
    laser.setOrigin(3, 240);
    laser.setFillColor(sf::Color(255,0,0,192));
    laser.setRotation(sf::vector2ToAngle(getCollisionLineVector()));
    laser.setPosition(getPosition());
    window.draw(laser);
}

void BreLaser::collision(Collisionable* other)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e)
        e->takeDamage(getPosition(), 1, 1);
}

MoneyShield::MoneyShield(P<BreEnemy> owner, float startAngle, float endDistance, bool counterClockwise)
: Collisionable(10), endDistance(endDistance), counterClockwise(counterClockwise)
{
    this->owner = owner;
    angle = startAngle;
    distance = 0;
    textureManager.setTexture(sprite, "Money");
}

void MoneyShield::update(float delta)
{
    if (!owner)
    {
        destroy();
        return;
    }
    
    if (counterClockwise)
        angle -= delta * 50.0f;
    else
        angle += delta * 50.0f;
    distance += delta * endDistance;
    if (distance > endDistance)
        distance = endDistance;

    setPosition(owner->getPosition() + sf::vector2FromAngle(angle) * distance);
}

void MoneyShield::render(sf::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.setRotation(angle);
    
    window.draw(sprite);
}

bool MoneyShield::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    destroy();
    P<ScoreManager>(engine->getObject("score"))->add(2);
    return true;
}
