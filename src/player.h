#ifndef PLAYER_H
#define PLAYER_H

#define MAX_PLAYERS 2

#include "gameEntity.h"
#include "Collisionable.h"
#include "input.h"
#include "random.h"

const int fireButton = 0;
const int chargeShotButton = 1;
const int nukeButton = 2;
const int skipButton = 3;
const int slowButton = 4;

extern bool playerBonusWeaponsActive;

class PlayerInfo
{
public:
    int lives;
    int nukes;
};

class PlayerBonusLaser;
class PlayerCraft: public GameEntity, public Collisionable
{
public:
    PlayerController* controller;
    PlayerInfo* info;
    sf::Vector2f velocity;
    int type;
    float fireCooldown, nukeCooldown;
    float chargeShot;
    static const float minChargeShot = 0.3;
    static const float maxChargeShot = 2.0;
    float invulnerability;
    int health;
    sf::Color color;
    P<PlayerBonusLaser> bonusLaser;
public:
    PlayerCraft(PlayerController* controller, PlayerInfo* info, int type);

    virtual ~PlayerCraft();

    virtual void update(float delta);

    virtual void render(sf::RenderTarget& window);

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
};

class PlayerBonusLaser: public GameEntity, public Collisionable
{
    P<PlayerCraft> owner;
    P<GameEntity> damageTarget;
    float length;
public:
    int type;
    
    PlayerBonusLaser(P<PlayerCraft> owner);
    virtual ~PlayerBonusLaser() {}

    virtual void update(float delta);
    virtual void render(sf::RenderTarget& window);

    virtual void collision(Collisionable* other);
};

#endif//PLAYER_H
