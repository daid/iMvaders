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
        if (shotDelay > 0)
            shotDelay -= delta;
        if (shotDelay <= 0)
        {
            shotDelay += 1.0/60.0;
            float a = sprite.getRotation();
            if (a > 180 - shotAngle/2 && a < 180 + shotAngle/2)
                new Bullet(sprite.getPosition() + sf::Vector2f(8.0f * sinf((shotsPerBurst - shots) / float(shotsPerBurst) * M_PI * 4), 4.0), 0, a, 360.0f);
            shots--;
            if (shots == 0)
                shotDelay = random(1.0, 8.0);
        }
    }
    else if (charge > 0)
    {
        charge -= delta;
        if (fmodf(charge, 4.0/60.0) > 2.0/60.0)
            sprite.setColor(sf::Color(255, 255, 255));
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
