#ifndef BASICENEMY_H
#define BASICENEMY_H
#include "basicEnemyBase.h"

class BasicEnemy : public BasicEnemyBase
{
private:
    float shotDelay;
    static const int shotAngle = 120;
public:
    BasicEnemy()
    {
        shotDelay = random(0.8, 8.0);
    }

    virtual ~BasicEnemy();

    virtual void update(float delta);
};

#endif // BASICENEMY_H
