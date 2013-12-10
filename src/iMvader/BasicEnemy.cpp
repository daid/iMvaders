#include "basicEnemy.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_SUBCLASS(BasicEnemy, BasicEnemyBase)
{
}

BasicEnemy::~BasicEnemy(){}

void BasicEnemy::update(float delta)
{
    if (shotDelay > 0)
    {
        shotDelay -= delta;
    }else{
        shotDelay = random(0.8, 8.0);
        float a = sprite.getRotation();
        if (a > 180 - shotAngle/2 && a < 180 + shotAngle/2)
            new Bullet(sprite.getPosition(), 0, a);
    }
    BasicEnemyBase::update(delta);
}
