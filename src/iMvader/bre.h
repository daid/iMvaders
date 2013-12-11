#ifndef BRE_H
#define BRE_H

#include "gameEntity.h"
#include "BasicEnemy.h"

enum BreState
{
    BS_FlyIn,
    BS_MoveLeftRight,
    BS_LaserCharge,
    BS_LaserFire,
    BS_MouthOpen,
    BS_MouthClose,
};

class BreEnemy: public GameEntity
{
public:
    sf::Sprite mouth;
    BreState state;
    int moveDir;
    int health;
    float shotDelay;
    float moveSpeed;
    float mouthPos;
    float invulnerability;
    int enemySpawnCount;
    PVector<BasicEnemyBase> enemyList;

    static const int maxHealth = 100;
    static const int normalShotDelay = 1.2;
    static const int laserChargeTime = 1.0;

    BreEnemy();
    
    virtual ~BreEnemy();

    virtual void update(float delta);

    virtual void render(sf::RenderTarget& window);

    virtual void postRender(sf::RenderTarget& window);

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
};
#endif // BRE_H
