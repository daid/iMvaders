#ifndef BURSTSHOTENEMY_H
#define BURSTSHOTENEMY_H
#include "basicEnemyBase.h"

class BurstShotEnemy : public BasicEnemyBase
{
private:
    float shotDelay;
    float charge;
    int shots;
    static const int shotAngle = 10;
    static const int shotsPerBurst = 60;
    static const float chargeUpTime = 1.0f;
public:
    BurstShotEnemy(sf::Vector2f targetPosition)
    : BasicEnemyBase(targetPosition)
    {
        shotDelay = random(1.0, 8.0);
        charge = 0;
        shots = 0;
    }

    virtual ~BurstShotEnemy();
    virtual void update(float delta);
};
#endif // BURSTSHOTENEMY_H
