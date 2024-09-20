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
    spriteManager.setTexture(sprite, "bre1");
    sprite.setPosition(glm::vec2(160, -80));
    spriteManager.setTexture(mouth, "bre2");
    mouth.setPosition(glm::vec2(160, -80));
    spriteManager.setTexture(shield, "bre_shield");
    state = BS_FlyIn;
    mouthPos = 0;
    shotDelay = 0;
    invulnerability = 0;
    moveSpeed = 60;
    moneyshieldDeployed = false;
    health = maxHealth;
    difficulty = 1;
    shieldStrength = 0;
}

BreEnemy::~BreEnemy()
{
}

void BreEnemy::setDifficulty(int difficulty)
{
    this->difficulty = difficulty;
    if (difficulty > 1)
        spriteManager.setTexture(sprite, "bre1_v2");
    if (difficulty > 2)
    {
        shieldStrength = maxShieldStrength;
        setCollisionRadius(65);
    }
}

void BreEnemy::update(float delta)
{
    if (invulnerability > 0)
        invulnerability -= delta;
    switch(state)
    {
    case BS_FlyIn:
        if (sprite.getPosition().y < 80.0f)
        {
            sprite.setPosition(sprite.getPosition() + glm::vec2(0, moveSpeed * delta));
        }else
        {
            new BreEnemyHud(this);
            state = BS_MoveLeftRight;
            moveDir = random(0, 100) < 50 ? 1 : -1;
        }
        break;
    case BS_MoveLeftRight:
        sprite.setPosition(sprite.getPosition() + glm::vec2(moveDir * moveSpeed * delta, 0));
        if (sprite.getPosition().x < 40.0f)
            moveDir = 1;
        if (sprite.getPosition().x > 280.0f)
            moveDir = -1;

        if (shotDelay > 0)
        {
            shotDelay -= delta;
            if (difficulty > 1)
                shotDelay -= delta * 0.1f;
        }else{
            if (random(0, 100) < 30)
            {
                if (random(0, 100) < 50)
                {
                    state = BS_LaserCharge;
                    shotDelay = laserChargeTime;
                }else{
                    state = BS_MouthOpen;
                    enemySpawnCount = 3 + 2 * difficulty;
                }
            }else{
                for(int n=-2; n<=2; n++)
                    new Bullet(sprite.getPosition() + glm::vec2(-n*15, -50), 0, 180 + n * 7, 90.0);
                shotDelay = normalShotDelay;
            }
        }
        break;
    case BS_LaserCharge:
        if (shotDelay > 0)
        {
            shotDelay -= delta;
            if (difficulty > 1)
                shotDelay -= delta * 0.3f;
        }else{
            state = BS_LaserFire;
            laser[0] = new BreLaser(this);
            laser[1] = new BreLaser(this);
            shotDelay = 0.5;
        }
        break;
    case BS_LaserFire:
        if (shotDelay > -0.5f) //Why -0.5?
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
            mouthPos += delta * 40.0f;
            if (difficulty > 1)
                mouthPos += delta * 5.0f;
            shotDelay = 0.2;
        }else if (shotDelay > 0)
        {
            shotDelay -= delta;
            if (difficulty > 1)
                shotDelay -= delta * 0.3f;
        }else{
            shotDelay += 0.2f;
            if (enemySpawnCount > 0)
            {
                BasicEnemyBase* e = new BasicEnemy();
                e->setTargetPosition(glm::vec2(random(20, 300), random(20, 200)));
                e->wait(sprite.getPosition() + glm::vec2(0, 50), sprite.getPosition() + glm::vec2(0, 100));
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
        break;
    }
    if (laser[0])
    {
        laser[0]->setPosition(getPosition() + glm::vec2(18, 7));
        laser[0]->setRotation(-shotDelay*30.0f);
    }
    if (laser[1])
    {
        laser[1]->setPosition(getPosition() + glm::vec2(-18, 7));
        laser[1]->setRotation(shotDelay*30.0f);
    }
    if (difficulty > 2 && shieldStrength < maxShieldStrength)
    {
        shieldCharge += delta;
        if (shieldCharge >= shieldChargeTime)
        {
            shieldCharge -= shieldChargeTime;
            if (shieldStrength == 0)
                setCollisionRadius(65);
            shieldStrength ++;
        }
    }
    
    mouth.setPosition(sprite.getPosition() + glm::vec2(0, mouthPos));
    shield.setPosition(sprite.getPosition());
    setPosition(sprite.getPosition()); // Set position for collision
    foreach(BasicEnemyBase, e, enemyList)
    {
        if (e->state == ES_CenterField)
            e->dive(glm::vec2(random(20, 300), 340));
        if (e->state == ES_Outside)
        {
            e->wait(glm::vec2(random(20, 300), -40), sprite.getPosition() + glm::vec2(0, 100));
            e->flyIn();
        }
    }
}

void BreEnemy::render(sp::RenderTarget& window)
{
    if (fmodf(invulnerability, 4.0f/60) > 2.0f/60.0f)
    {
        if (shieldStrength > 0)
        {
            shield.setColor({255,255,255,128});
        }else{
            sprite.setColor({212, 0, 0, 255});
            mouth.setColor({212, 0, 0, 255});
        }
    }
    else if (state == BS_LaserCharge)
    {
        float f = (shotDelay / laserChargeTime);
        sprite.setColor({212, 128 * f, 128 * f, 255});
        mouth.setColor({212, 128 * f, 128 * f, 255});
    }
    else
    {
        sprite.setColor({255,255,255,255});
        mouth.setColor({255,255,255,255});
        shield.setColor({255,255,255,255});
    }
    sprite.draw(window);
    mouth.draw(window);
    if (shieldStrength > 0)
        shield.draw(window);
}

BreEnemyHud::BreEnemyHud(P<BreEnemy> owner)
: owner(owner)
{
}

void BreEnemyHud::update(float delta)
{
    if (!owner)
        destroy();
}

void BreEnemyHud::render(sp::RenderTarget& window)
{
    if (!owner)
        return;

    RectangleShape healthBarBG(glm::vec2(280, 10));
    healthBarBG.setFillColor({0,0,0,0});
    healthBarBG.setOutlineColor({128, 128, 128, 128});
    healthBarBG.setOutlineThickness(1);
    healthBarBG.setPosition(20, 10);
    healthBarBG.draw(window);

    RectangleShape healthBar(glm::vec2(280 * owner->health / BreEnemy::maxHealth, 10));
    healthBar.setFillColor({212, 0, 0, 128});
    healthBar.setPosition(20, 10);
    healthBar.draw(window);
    
    if (owner->difficulty > 2)
    {
        RectangleShape shieldBarBG(glm::vec2(280, 8));
        shieldBarBG.setFillColor({0,0,0,0});
        shieldBarBG.setOutlineColor({128, 128, 128, 128});
        shieldBarBG.setOutlineThickness(1);
        shieldBarBG.setPosition(20, 25);
        shieldBarBG.draw(window);

        RectangleShape shieldBar(glm::vec2(280 * owner->shieldStrength / BreEnemy::maxShieldStrength, 8));
        shieldBar.setFillColor({100, 100, 212, 192});
        shieldBar.setPosition(20, 25);
        shieldBar.draw(window);
        
        if (owner->shieldStrength < BreEnemy::maxShieldStrength)
        {
            RectangleShape shieldCharge(glm::vec2(280 * owner->shieldCharge / BreEnemy::shieldChargeTime, 3));
            shieldCharge.setFillColor({100, 100, 212, 192});
            shieldCharge.setPosition(20, 30);
            shieldCharge.draw(window);
        }
    }
}

bool BreEnemy::takeDamage(glm::vec2 position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    if (invulnerability > 0)
        return true;

    if (state != BS_FlyIn)
    {
        if (shieldStrength > 0)
        {
            shieldStrength -= damageAmount;
            damageAmount = 0;
            if (shieldStrength <= 0)
            {
                setCollisionRadius(50);
                damageAmount = -shieldStrength;
                shieldStrength = 0;
            }
        }
        shieldCharge = 0.0;
        health -= damageAmount;
    }
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
        for(unsigned int n=0; n<4; n++)
        {
            new BreDeath(getPosition(), n);
        }
        foreach(BasicEnemyBase, e, enemyList)
            e->destroy();
        P<ScoreManager>(engine->getObject("score"))->add(500);
        if (difficulty > 1)
            P<ScoreManager>(engine->getObject("score"))->add(200);
        for(unsigned int n=0; n<30; n++)
            new Explosion(sprite.getPosition() + glm::vec2(random(-60, 60), random(-80, 80)), 15);
    }
    invulnerability = 0.10;
    return true;
}

BreDeath::BreDeath(glm::vec2 position, int index)
{
    switch(index)
    {
    case 0:
        spriteManager.setTexture(sprite, "bre_death1");
        velocity = glm::vec2(random(-10, 10), random(40, 50));
        break;
    case 1:
        spriteManager.setTexture(sprite, "bre_death2");
        velocity = glm::vec2(random(40, 50), random(-10, 10));
        break;
    case 2:
        spriteManager.setTexture(sprite, "bre_death3");
        velocity = glm::vec2(random(-10, 10), random(-50, -40));
        break;
    case 3:
        spriteManager.setTexture(sprite, "bre_death4");
        velocity = glm::vec2(random(-50, -40), random(-10, 10));
        break;
    }
    angularVelocity = random(-30, 30);
    sprite.setPosition(position);
    lifeTime = 1.0;
    engine->setGameSpeed(0.3);
}

void BreDeath::update(float delta)
{
    lifeTime -= delta;
    if (lifeTime < 0)
    {
        engine->setGameSpeed(1.0);
        destroy();
    }
    sprite.setPosition(sprite.getPosition() + velocity * delta);
    sprite.setRotation(sprite.getRotation() + angularVelocity * delta);
    if (lifeTime < 1.0f)
        sprite.setColor({255,255,255, 255 * lifeTime});
    velocity *= powf(0.5, delta);
    angularVelocity *= powf(0.5, delta);
}

void BreDeath::render(sp::RenderTarget& window)
{
    sprite.draw(window);
}

BreLaser::BreLaser(P<BreEnemy> owner)
: Collisionable(glm::vec2(3, 240), glm::vec2(1.5, 120))
{
    this->owner = owner;
}

void BreLaser::update(float delta)
{
    if (!owner)
        destroy();
}

void BreLaser::render(sp::RenderTarget& window)
{
    RectangleShape laser(glm::vec2(6, 240));
    laser.setOrigin(3, 0);
    laser.setFillColor({255,0,0,192});
    laser.setRotation(getRotation());
    laser.setPosition(getPosition());
    laser.draw(window);
}

void BreLaser::collide(Collisionable* other, float force)
{
    GameEntity* e = dynamic_cast<GameEntity*>(other);
    if (e)
        e->takeDamage(getPosition(), 1, 1);
}

MoneyShield::MoneyShield(P<BreEnemy> owner, float startAngle, float endDistance, bool counterClockwise)
: Collisionable(glm::vec2(31, 15)), endDistance(endDistance), counterClockwise(counterClockwise)
{
    this->owner = owner;
    setRotation(startAngle);
    distance = 0;
    spriteManager.setTexture(sprite, "Money");
}

void MoneyShield::update(float delta)
{
    if (!owner)
    {
        destroy();
        return;
    }
    
    if (counterClockwise)
        setRotation(getRotation() - delta * 50.0f);
    else
        setRotation(getRotation() + delta * 50.0f);
    distance += delta * endDistance;
    if (distance > endDistance)
        distance = endDistance;

    setPosition(owner->getPosition() + rotateVec2(glm::vec2(0, -1), getRotation()) * distance);
}

void MoneyShield::render(sp::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    sprite.setRotation(getRotation());
    
    sprite.draw(window);
}

bool MoneyShield::takeDamage(glm::vec2 position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    destroy();
    P<ScoreManager>(engine->getObject("score"))->add(2);
    return true;
}
