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

    void add(BasicEnemyBase* e);

    virtual void update(float delta) override;
    
    void setOffset(float offset);

    //return true when all the enemies in this group in this state.
    bool isAll(EnemyState state);

    void dive(glm::vec2 target);

    void flyIn(glm::vec2 start);

    void flyInBy(glm::vec2 start, glm::vec2 flyByPoint);
};

#endif // ENEMYGROUP_H
