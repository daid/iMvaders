#include "basicEnemy.h"

BasicEnemy::~BasicEnemy(){}

void BasicEnemy::update(float delta)
{
    if (shotDelay > 0)
    {
        shotDelay -= delta; //This is *bad* Never use frame based timing! The time should be Mili-seconds based.
    }else{
        shotDelay = random(0.8, 8.0);
        float a = sprite.getRotation();
        if (a > 180 - shotAngle/2 && a < 180 + shotAngle/2)
            new Bullet(sprite.getPosition(), 0, a);
    }
    BasicEnemyBase::update(delta);
}
