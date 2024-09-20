#include "BasicEnemyBase.h"

class DiveBomber : public BasicEnemyBase
{
    float fireDelay;
public:
    virtual void update(float delta) override
    {
        BasicEnemyBase::update(delta);

        switch(state)
        {
        case ES_Wait:
            break;
        case ES_FlyIn:
            if (fireDelay > 0)
            {
                fireDelay -= delta;
                if (fireDelay <= 0)
                    new Bullet(getPosition(), 0, getRotation(), 250);
            }
            break;
        case ES_CenterField:
        case ES_Diving:
        case ES_Outside:
            {
                glm::vec2 start(random(0, 320), -20);
                glm::vec2 end(start.x + random(-100, 100), 300);
                setTargetPosition(end);
                wait(start);
                flyIn();
                fireDelay = 0.5;
            }
            break;
        }
    }
};

REGISTER_SCRIPT_SUBCLASS(DiveBomber, BasicEnemyBase)
{
}
