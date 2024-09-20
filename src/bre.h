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

class BreLaser;
class BreEnemy: public GameEntity, public Collisionable
{
public:
    Sprite mouth;
    Sprite shield;
    BreState state;
    int moveDir;
    int health;
    float shotDelay;
    float moveSpeed;
    float mouthPos;
    float invulnerability;
    int enemySpawnCount;
    int difficulty;
    int shieldStrength;
    float shieldCharge;
    bool moneyshieldDeployed;
    PVector<BasicEnemyBase> enemyList;
    P<BreLaser> laser[2];
    ScriptCallback destroyed;

    static const int maxHealth = 100;
    static const int maxShieldStrength = 10;
    static const int shieldChargeTime = 2.5;
    static const int normalShotDelay = 1.2;
    static const int laserChargeTime = 2.0;

    BreEnemy();
    virtual ~BreEnemy();

    void setDifficulty(int difficulty);
    
    virtual void update(float delta) override;
    virtual void render(sp::RenderTarget& window) override;
    virtual bool takeDamage(glm::vec2 position, int damageType, int damageAmount) override;
};

class BreEnemyHud: public GameEntity
{
public:
    P<BreEnemy> owner;
    
    BreEnemyHud(P<BreEnemy> owner);
    
    virtual void update(float delta) override;
    virtual void render(sp::RenderTarget& window) override;
};

class BreDeath: public GameEntity
{
    float lifeTime;
    glm::vec2 velocity{};
    float angularVelocity;
public:
    BreDeath(glm::vec2 position, int index);
    
    virtual void update(float delta) override;
    virtual void render(sp::RenderTarget& window) override;
};

class BreLaser: public GameEntity, public Collisionable
{
    P<BreEnemy> owner;
public:
    BreLaser(P<BreEnemy> owner);
    virtual ~BreLaser() {}

    virtual void update(float delta) override;
    virtual void render(sp::RenderTarget& window) override;

    virtual void collide(Collisionable* other, float force) override;
};

class MoneyShield: public GameEntity, public Collisionable
{
    float distance;
    float endDistance;
    bool counterClockwise;
    
    P<BreEnemy> owner;
public:
    MoneyShield(P<BreEnemy> owner, float startAngle, float endDistance, bool counterClockwise);
    virtual ~MoneyShield() {}
    
    virtual void update(float delta) override;
    virtual void render(sp::RenderTarget& window) override;
    
    virtual bool takeDamage(glm::vec2 position, int damageType, int damageAmount) override;
};

#endif // BRE_H
