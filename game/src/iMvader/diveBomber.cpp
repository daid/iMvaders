#include "BasicEnemyBase.h"

class DiveBomber : public BasicEnemyBase
{
    float fireDelay;
public:
    virtual void update(float delta)
    {
        BasicEnemyBase::update(delta);

        switch(state)
        {
        case ES_Wait:
            printf("Wait\n");
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
            printf("ES_Outside\n");
            {
                sf::Vector2f start(random(0, 320), -20);
                sf::Vector2f end(start.x + random(-100, 100), 300);
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
