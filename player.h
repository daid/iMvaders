#ifndef PLAYER_H
#define PLAYER_H

#define MAX_PLAYERS 2

#include "gameEntity.h"

class PlayerController;
extern PlayerController playerController[MAX_PLAYERS];

class PlayerController: public sf::NonCopyable
{
public:
    sf::Keyboard::Key keyBind[5];
    
    PlayerController()
    {
        keyBind[0] = sf::Keyboard::Left;
        keyBind[1] = sf::Keyboard::Right;
        keyBind[2] = sf::Keyboard::Up;
        keyBind[3] = sf::Keyboard::Down;
        keyBind[4] = sf::Keyboard::Space;
    }

    bool left() { return sf::Keyboard::isKeyPressed(keyBind[0]); }
    bool right() { return sf::Keyboard::isKeyPressed(keyBind[1]); }
    bool up() { return sf::Keyboard::isKeyPressed(keyBind[2]); }
    bool down() { return sf::Keyboard::isKeyPressed(keyBind[3]); }
    bool fire() { return sf::Keyboard::isKeyPressed(keyBind[4]); }
};

class PlayerCraft: public GameEntity
{
public:
    PlayerController* controller;
    sf::Vector2f velocity;
    int type;
    float fireCooldown;
    float invulnerability;
public:
    PlayerCraft(PlayerController* controller, int type);

    virtual ~PlayerCraft();

    virtual void update(float delta);

    virtual void render(sf::RenderTarget& window);

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
};

#endif//PLAYER_H
