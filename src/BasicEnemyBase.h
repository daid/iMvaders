#ifndef BASICENEMYBASE_H
#define BASICENEMYBASE_H

#include "curve.h"
#include "engine.h"
#include "random.h"
#include "textureManager.h"
#include "explosion.h"
#include "bullet.h"
#include "scriptInterface.h"


enum EnemyState
{
    ES_Wait,
    ES_FlyIn,
    ES_CenterField,
    ES_Diving,
    ES_Outside
};

/* Define script conversion function for the EnemyState enum. */
template<> void convert<EnemyState>::param(lua_State* L, int& idx, EnemyState& es);

class BasicEnemyBase: public GameEntity, public Collisionable
{
public:
    EnemyState state;
    Curve flyInCurve[2];
    Curve diveCurve;
    glm::vec2 targetPosition;
    bool hasShield;
    float shieldPower;
    int flyIncurveNr, flyIncurveCount;
    static constexpr float shieldMaxPower = 0.5;
    static constexpr float flySpeed = 120.0;
    float enemyOffset;
    glm::u8vec4 color;

public:
    BasicEnemyBase()
    : GameEntity(), Collisionable(8.0f)
    {
        state = ES_Outside;
        flyIncurveCount = 0;
        enemyOffset = 0;

        spriteManager.setTexture(sprite, "BasicEnemy", 0);
        color = {212, 0, 0, 255};
        sprite.setColor(color);
        setPosition(glm::vec2(-50, -50));
        hasShield = false;
    }
    virtual ~BasicEnemyBase();
    virtual void update(float delta) override;
    virtual void collide(Collisionable* target, float force) override;
    
    void setTargetPosition(glm::vec2 targetPosition) { this->targetPosition = targetPosition; }

    void dive(glm::vec2 target);
    void wait(glm::vec2 start);
    void wait(glm::vec2 start, glm::vec2 flyByTarget);
    void flyIn();
    void giveShield();

    virtual void render(sp::RenderTarget& window) override;
    bool shieldUp();

    virtual bool takeDamage(glm::vec2 position, int damageType, int damageAmount) override;
};
#endif
