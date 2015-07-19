#ifndef BURSTSHOTENEMY_H
#define BURSTSHOTENEMY_H
#include "BasicEnemyBase.h"

class BurstShotEnemy : public BasicEnemyBase
{
private:
    float shotDelay;
    float charge;
    int shots;
    static constexpr int shotAngle = 10;
    static constexpr int shotsPerBurst = 60;
    static constexpr float chargeUpTime = 1.0f;
public:
    BurstShotEnemy()
    {
        shotDelay = random(1.0, 8.0);
        charge = 0;
        shots = 0;
    }

    virtual ~BurstShotEnemy();
    virtual void update(float delta);
};
#endif // BURSTSHOTENEMY_H
