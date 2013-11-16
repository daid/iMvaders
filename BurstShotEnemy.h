#ifndef BURSTSHOTENEMY_H
#define BURSTSHOTENEMY_H
#include "basicEnemyBase.h"

class BurstShotEnemy : public BasicEnemyBase
{
private:
    int shotDelay;
    int charge;
    int shots;
    static const int shotAngle = 10;
    static const int shotsPerBurst = 60;
public:
    BurstShotEnemy(sf::Vector2f targetPosition)
    : BasicEnemyBase(targetPosition)
    {
        shotDelay = random(50, 500);
        charge = 0;
        shots = 0;
    }

    virtual ~BurstShotEnemy();
    virtual void update();
};
#endif // BURSTSHOTENEMY_H
