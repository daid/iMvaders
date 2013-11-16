#ifndef BASICENEMY_H
#define BASICENEMY_H
#include "basicEnemyBase.h"

class BasicEnemy : public BasicEnemyBase
{
private:
    int shotDelay;
    static const int shotAngle = 120;
public:
    BasicEnemy(sf::Vector2f targetPosition)
    : BasicEnemyBase(targetPosition)
    {
        shotDelay = random(50, 500);
    }

    virtual ~BasicEnemy();

    virtual void update();
};

#endif // BASICENEMY_H
