#ifndef ENEMYGROUP_H
#define ENEMYGROUP_H

#include "gameEntity.h"
#include "BasicEnemyBase.h"
#include "BasicEnemy.h"
#include "BurstShotEnemy.h"
class EnemyGroup : public GameEntity
{
private:
    PVector<BasicEnemyBase> enemyList;
public:
    EnemyGroup();
    virtual ~EnemyGroup();

    BasicEnemyBase* add(sf::Vector2f targetPoint);

    virtual void update(float delta);
    
    void setOffset(float offset);

    //return true when all the enemies in this group in this state.
    bool isAll(EnemyState state);

    void dive(sf::Vector2f target);

    void flyIn(sf::Vector2f start);

    void flyIn(sf::Vector2f start, sf::Vector2f flyByPoint);
};


#endif // ENEMYGROUP_H
