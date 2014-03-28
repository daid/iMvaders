#ifndef PLAYER_H
#define PLAYER_H

#define MAX_PLAYERS 2

#include "gameEntity.h"
#include "Collisionable.h"
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

class PlayerController: public virtual PObject
{
public:
    static const int buttonCount = 6;
    sf::Keyboard::Key keyBind[4 + buttonCount];

    PlayerController()
    {
        keyBind[0] = sf::Keyboard::Left;
        keyBind[1] = sf::Keyboard::Right;
        keyBind[2] = sf::Keyboard::Up;
        keyBind[3] = sf::Keyboard::Down;
        keyBind[4] = sf::Keyboard::Space;
        keyBind[5] = sf::Keyboard::Z;
        keyBind[6] = sf::Keyboard::X;
        keyBind[7] = sf::Keyboard::C;
        keyBind[8] = sf::Keyboard::V;
        keyBind[9] = sf::Keyboard::B;
    }
    virtual ~PlayerController() {}

    bool left() { return sf::Keyboard::isKeyPressed(keyBind[0]); }
    bool right() { return sf::Keyboard::isKeyPressed(keyBind[1]); }
    bool up() { return sf::Keyboard::isKeyPressed(keyBind[2]); }
    bool down() { return sf::Keyboard::isKeyPressed(keyBind[3]); }
    bool button(int idx) { return sf::Keyboard::isKeyPressed(keyBind[4 + idx]); }
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
public:
    int type;
    
    PlayerBonusLaser(P<PlayerCraft> owner);
    virtual ~PlayerBonusLaser() {}

    virtual void update(float delta);
    virtual void preRender(sf::RenderTarget& window);

    virtual void collision(Collisionable* other);
};

#endif//PLAYER_H
