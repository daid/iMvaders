#include "BurstShotEnemy.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_SUBCLASS(BurstShotEnemy, BasicEnemyBase)
{
}

BurstShotEnemy::~BurstShotEnemy() {}

void BurstShotEnemy::update(float delta)
{
    if (shots)
    {
        if (shotDelay > 0.0f)
            shotDelay -= delta;
        if (shotDelay <= 0)
        {
            shotDelay += 1.0f/60.0f;
            float a = sprite.getRotation();
            if (a > 180 - shotAngle/2 && a < 180 + shotAngle/2)
                new Bullet(getPosition() + glm::vec2(8.0f * sinf((shotsPerBurst - shots) / float(shotsPerBurst) * float(M_PI) * 4.0f), 4.0f), 0, a, 360.0f);
            shots--;
            if (shots == 0)
                shotDelay = random(1.0, 8.0);
        }
    }
    else if (charge > 0)
    {
        charge -= delta;
        if (fmodf(charge, 4.0f/60.0f) > 2.0f/60.0f)
            sprite.setColor({255,255,255,255});
        else
            sprite.setColor(color);
        if (charge <= 0)
        {
            shots = shotsPerBurst;
            sprite.setColor(color);
        }
    }
    else if (shotDelay > 0)
    {
        shotDelay -= delta;
    }else{
        if (state == ES_CenterField)
        {
            charge = chargeUpTime;
            shotDelay = 0.0;
        }else{
            shotDelay = random(1.0, 8.0);
        }
    }
    BasicEnemyBase::update(delta);
}
