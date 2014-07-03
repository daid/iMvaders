#include <string.h>
#include "EnemyGroup.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(EnemyGroup)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(EnemyGroup, add);
    REGISTER_SCRIPT_CLASS_FUNCTION(EnemyGroup, dive);
    REGISTER_SCRIPT_CLASS_FUNCTION(EnemyGroup, flyIn);
    REGISTER_SCRIPT_CLASS_FUNCTION(EnemyGroup, flyInBy);
    REGISTER_SCRIPT_CLASS_FUNCTION(EnemyGroup, isAll);
    REGISTER_SCRIPT_CLASS_FUNCTION(EnemyGroup, setOffset);
}

EnemyGroup::EnemyGroup(){}

EnemyGroup::~EnemyGroup(){}

void EnemyGroup::add(BasicEnemyBase* e)
{
    enemyList.push_back(e);
}

void EnemyGroup::setOffset(float offset)
{
    foreach(BasicEnemyBase, e, enemyList)
    {
        e->enemyOffset = offset;
    }
}

void EnemyGroup::update(float delta)
{
    if (enemyList.size() < 1)
        destroy();

    P<BasicEnemyBase> prev;
    foreach(BasicEnemyBase, e, enemyList)
    {
        if (e->state == ES_Wait)
        {
            if (!prev || e->sprite.getPosition() - prev->sprite.getPosition() > 20.0f)
                e->flyIn();
            break;
        }
        if (e->state == ES_CenterField && prev && prev->state == ES_Diving)
        {
            if (e->sprite.getPosition() - prev->sprite.getPosition() > 30.0f)
                e->dive(prev->diveCurve.p1);
            break;
        }
        prev = e;
    }
}

bool EnemyGroup::isAll(EnemyState state)
{
    foreach(BasicEnemyBase, e, enemyList)
        if (e->state != state)
            return false;
    return true;
}

void EnemyGroup::dive(sf::Vector2f target)
{
    foreach(BasicEnemyBase, e, enemyList)
    {
        e->dive(target);
        break;
    }
}

void EnemyGroup::flyIn(sf::Vector2f start)
{
    foreach(BasicEnemyBase, e, enemyList)
        e->wait(start);
    if (enemyList.size() > 0)
        enemyList[0]->flyIn();
}

void EnemyGroup::flyInBy(sf::Vector2f start, sf::Vector2f flyByPoint)
{
    foreach(BasicEnemyBase, e, enemyList)
        e->wait(start, flyByPoint);
    if (enemyList.size() > 0)
        enemyList[0]->flyIn();
}
