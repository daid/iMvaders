#include <string.h>
#include "basicEnemyBase.h"
#include "scoreManager.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(BasicEnemyBase)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(BasicEnemyBase, giveShield);
    REGISTER_SCRIPT_CLASS_FUNCTION(BasicEnemyBase, setTargetPosition);
}

/* Define script conversion function for the EnemyState enum. */
template<> void convert<EnemyState>::param(lua_State* L, int& idx, EnemyState& es)
{
    if (lua_isnumber(L, idx))
    {
        int n = int(luaL_checknumber(L, idx++) + 0.5);
        if (n == 0)
            es = ES_Wait;
        else if (n == 1)
            es = ES_FlyIn;
        else if (n == 2)
            es = ES_CenterField;
        else if (n == 3)
            es = ES_Diving;
        else
            es = ES_Outside;
    }else{
        const char* str = luaL_checkstring(L, idx++);
        if (strcasecmp(str, "Wait") == 0)
            es = ES_Wait;
        else if (strcasecmp(str, "FlyIn") == 0)
            es = ES_FlyIn;
        else if (strcasecmp(str, "CenterField") == 0)
            es = ES_CenterField;
        else if (strcasecmp(str, "Diving") == 0)
            es = ES_Diving;
        else
            es = ES_Outside;
    }
}

BasicEnemyBase::~BasicEnemyBase(){}

void BasicEnemyBase::update(float delta)
{
    if (hasShield && shieldPower < shieldMaxPower)
    {
        shieldPower += delta;
        if (shieldPower >= shieldMaxPower)
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
            flyInCurve[flyIncurveNr].moveDistance(flySpeed * delta);
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
        setPosition(flyInCurve[flyIncurveNr].getPosition());
        break;
    case ES_CenterField:
        setPosition(targetPosition + sf::Vector2f(enemyOffset, 0));
        break;
    case ES_Diving:
        if (diveCurve.delta < 1.0)
        {
            diveCurve.moveDistance(flySpeed * delta);
            sprite.setRotation(diveCurve.angle());
        }
        else
        {
            state = ES_Outside;
        }
        setPosition(diveCurve.getPosition());
        break;
    case ES_Outside:
        break;
    }
}
void BasicEnemyBase::dive(sf::Vector2f target)
{
    diveCurve.p0 = getPosition();
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
    setPosition(flyInCurve[0].p0);
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
    setPosition(flyInCurve[0].p0);
}

void BasicEnemyBase::flyIn()
{
    state = ES_FlyIn;
}

void BasicEnemyBase::giveShield()
{
    setCollisionRadius(12.0f);
    hasShield = true;
    shieldPower = shieldMaxPower;
    textureManager.setTexture(sprite, "BasicEnemy", 1);
}

void BasicEnemyBase::render(sf::RenderTarget& window)
{
    sprite.setPosition(getPosition());
    window.draw(sprite);

#ifdef DEBUG
    for(int n=0; n<flyIncurveCount; n++)
        flyInCurve[n].draw(window);
    diveCurve.draw(window);
#endif
}
void BasicEnemyBase::collision(Collisionable* target)
{
    GameEntity* e = dynamic_cast<GameEntity*>(target);
    if (e)
        e->takeDamage(getPosition(), 0, 1);
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
        setCollisionRadius(8.0f);
        textureManager.setTexture(sprite, "BasicEnemy", 0);
    }
    else
    {
        new Explosion(getPosition(), 8);
        destroy();
        score.add(10);
    }
    return true;
}
