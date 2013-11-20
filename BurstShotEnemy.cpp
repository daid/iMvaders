#include "BurstShotEnemy.h"

BurstShotEnemy::~BurstShotEnemy() {}

void BurstShotEnemy::update()
{
    if (shots)
    {
        float a = sprite.getRotation();
        if (a > 180 - shotAngle/2 && a < 180 + shotAngle/2)
            new Bullet(sprite.getPosition() + sf::Vector2f(8.0f * sinf((shotsPerBurst - shots) / float(shotsPerBurst) * M_PI * 4), 4.0), 0, a, 6.0f);
        shots--;
    }
    else if (charge)
    {
        charge--;
        if (charge & 2)
            sprite.setColor(sf::Color(255, 255, 255));
        else
            sprite.setColor(color);
        if (charge == 0)
            shots = shotsPerBurst;
    }
    else if (shotDelay)
    {
        shotDelay--;
    }else{
        shotDelay = random(400, 600);
        float a = sprite.getRotation();
        if (a > 180 - shotAngle/2 && a < 180 + shotAngle/2)
            charge = 60;
    }
    BasicEnemyBase::update();
}
