#include "basicEnemy.h"

BasicEnemy::~BasicEnemy(){}

void BasicEnemy::update()
{
    if (shotDelay)
    {
        shotDelay--; //This is *bad* Never use frame based timing! The time should be Mili-seconds based.
    }else{
        shotDelay = random(200, 400);
        float a = sprite.getRotation();
        if (a > 180 - shotAngle/2 && a < 180 + shotAngle/2)
            new Bullet(sprite.getPosition(), 0, a);
    }
    BasicEnemyBase::update();
}
