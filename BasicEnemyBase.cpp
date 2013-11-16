#include "basicEnemyBase.h"
BasicEnemyBase::~BasicEnemyBase(){}

void BasicEnemyBase::update()
{
    if (hasShield && shieldPower < shieldMaxPower)
    {
        shieldPower++;
        if (shieldPower == shieldMaxPower)
            giveShield();
    }

    if (flyIncurveCount > 0)
        flyInCurve[flyIncurveCount-1].p1 = targetPosition + sf::Vector2f(enemyOffset, 0);
    diveCurve.p0 = targetPosition + sf::Vector2f(enemyOffset, 0);
    switch(state)
    {
    case ES_Wait:
        break;
    case ES_FlyIn:
        if (flyInCurve[flyIncurveNr].delta < 1.0)
        {
            flyInCurve[flyIncurveNr].moveDistance(2.0);
            sprite.setRotation(flyInCurve[flyIncurveNr].angle());
        }
        else
        {
            if (flyIncurveNr < flyIncurveCount - 1)
            {
                flyIncurveNr++;
                flyInCurve[flyIncurveNr].delta = 0.0;
            }else{
                sprite.setRotation(180);
                state = ES_CenterField;
            }
        }
        sprite.setPosition(flyInCurve[flyIncurveNr].getPosition());
        break;
    case ES_CenterField:
        sprite.setPosition(targetPosition + sf::Vector2f(enemyOffset, 0));
        break;
    case ES_Diving:
        if (diveCurve.delta < 1.0)
        {
            diveCurve.moveDistance(2.0);
            sprite.setRotation(diveCurve.angle());
        }
        else
        {
            state = ES_Outside;
        }
        sprite.setPosition(diveCurve.getPosition());
        break;
    case ES_Outside:
        break;
    }
}
void BasicEnemyBase::dive(sf::Vector2f target)
{
    diveCurve.p0 = sprite.getPosition();
    diveCurve.cp0 = diveCurve.p0 + sf::vector2FromAngle(sprite.getRotation()) * 30.0f;
    diveCurve.p1 = target;
    diveCurve.cp1 = sf::Vector2f(target.x, 180);
    diveCurve.delta = 0.0;
    state = ES_Diving;
}

void BasicEnemyBase::wait(sf::Vector2f start)
{
    state = ES_Wait;
    flyIncurveNr = 0;
    flyInCurve[0].delta = 0.0;
    flyInCurve[0].p0 = start;
    flyInCurve[0].cp0 = flyInCurve[0].p0 + sf::Vector2f(0, 30);
    flyInCurve[0].p1 = targetPosition;
    flyInCurve[0].cp1 = flyInCurve[0].p1 - sf::Vector2f(0, 30);
    flyIncurveCount = 1;
    sprite.setPosition(flyInCurve[0].p0);
}
void BasicEnemyBase::wait(sf::Vector2f start, sf::Vector2f flyByTarget)
{
    state = ES_Wait;

    sf::Vector2f normal = normalize(flyByTarget - start);
    sf::Vector2f dir(-normal.y, normal.x);
    if (start.x > flyByTarget.x)
        dir = -dir;

    flyIncurveNr = 0;
    flyInCurve[0].delta = 0.0;
    flyInCurve[0].p0 = start;
    flyInCurve[0].cp0 = flyInCurve[0].p0 + sf::Vector2f(0, 30);
    flyInCurve[0].p1 = flyByTarget;
    flyInCurve[0].cp1 = flyByTarget + dir * 50.f;

    flyInCurve[1].p0 = flyByTarget;
    flyInCurve[1].cp0 = flyByTarget - dir * 50.f;
    flyInCurve[1].p1 = targetPosition;
    flyInCurve[1].cp1 = flyInCurve[1].p1 - sf::Vector2f(0, 50);
    flyIncurveCount = 2;
    sprite.setPosition(flyInCurve[0].p0);
}

void BasicEnemyBase::flyIn()
{
    state = ES_FlyIn;
}

void BasicEnemyBase::giveShield()
{
    collisionRadius = 12.0f;
    hasShield = true;
    shieldPower = shieldMaxPower;
    textureManager.setTexture(sprite, "BasicEnemy", 1);
}

void BasicEnemyBase::render(sf::RenderTarget& window)
{
    window.draw(sprite);

#ifdef DEBUG
    for(int n=0; n<flyIncurveCount; n++)
        flyInCurve[n].draw(window);
    diveCurve.draw(window);
#endif
}
bool BasicEnemyBase::shieldUp()
{
    return hasShield && shieldPower == shieldMaxPower;
}

bool BasicEnemyBase::takeDamage(sf::Vector2f position, int damageType, int damageAmount)
{
    if (damageType >= 0)
        return false;
    if (shieldUp())
    {
        shieldPower = 0;
        collisionRadius = 8.0f;
        textureManager.setTexture(sprite, "BasicEnemy", 0);
    }
    else
    {
        new Explosion(sprite.getPosition(), 8);
        destroy();
        //addScore(10); //A proper score manager is needed.
    }
    return true;
}
