#ifndef BRE_H
#define BRE_H

#include "gameEntity.h"

enum BreState
{
    BS_FlyIn,
    BS_MoveLeftRight,
    BS_LaserCharge,
    BS_LaserFire,
};

class BreEnemy: public GameEntity
{
public:
    sf::Sprite mouth;
    BreState state;
    int moveDir;
    int health;
    int shotDelay;
    float mouthPos;
    int invulnerability;
    static const int maxHealth = 100;
    static const int normalShotDelay = 120;
    static const int laserChargeTime = 60;

    BreEnemy()
    : GameEntity(50)
    {
        textureManager.setTexture(sprite, "bre1");
        sprite.setPosition(sf::Vector2f(160, -80));
        textureManager.setTexture(mouth, "bre2");
        mouth.setPosition(sf::Vector2f(160, -80));
        state = BS_FlyIn;
        mouthPos = 0;
        shotDelay = 0;
        invulnerability = 0;
        health = maxHealth;
    }
    
    virtual ~BreEnemy()
    {
    }

    virtual void update()
    {
        if (invulnerability)
            invulnerability--;
        switch(state)
        {
        case BS_FlyIn:
            if (sprite.getPosition().y < 80.0)
                sprite.setPosition(sprite.getPosition() + sf::Vector2f(0, 1));
            else{
                state = BS_MoveLeftRight;
                moveDir = random(0, 100) < 50 ? 1 : -1;
            }
            break;
        case BS_MoveLeftRight:
            sprite.setPosition(sprite.getPosition() + sf::Vector2f(moveDir, 0));
            if (sprite.getPosition().x < 40.0)
                moveDir = 1;
            if (sprite.getPosition().x > 280.0)
                moveDir = -1;
            
            if (shotDelay)
            {
                shotDelay--;
            }else{
                if (random(0, 100) < 20)
                {
                    state = BS_LaserCharge;
                    shotDelay = laserChargeTime;
                }else{
                    for(int n=-15; n<=15; n+= 5)
                        new Bullet(sprite.getPosition() + sf::Vector2f(-n*3, -50), 0, 180 + n, 1.5f);
                    shotDelay = normalShotDelay;
                }
            }
            break;
        case BS_LaserCharge:
            if (shotDelay)
            {
                shotDelay--;
            }else{
                state = BS_LaserFire;
                shotDelay = 30;
            }
            break;
        case BS_LaserFire:
            if (shotDelay > -30)
            {
                shotDelay--;
            }
            else
            {
                shotDelay = normalShotDelay;
                state = BS_MoveLeftRight;
            }
            break;
        }
        mouth.setPosition(sprite.getPosition() + sf::Vector2f(0, mouthPos));
    }

    virtual void render(sf::RenderTarget& window)
    {
        if ((invulnerability & 2) || (state == BS_LaserCharge && (shotDelay & 1)))
        {
            sprite.setColor(sf::Color(212, 0, 0));
            mouth.setColor(sf::Color(212, 0, 0));
        }
        else
        {
            sprite.setColor(sf::Color::White);
            mouth.setColor(sf::Color::White);
        }
        window.draw(sprite);
        window.draw(mouth);

        if (state == BS_LaserFire)
        {
            sf::RectangleShape laser(sf::Vector2f(6, 240));
            laser.setOrigin(3, 0);
            laser.setFillColor(sf::Color(255,0,0,192));
            laser.setRotation(-shotDelay/2);
            laser.setPosition(sprite.getPosition() + sf::Vector2f(18, 7));
            window.draw(laser);

            laser.setFillColor(sf::Color(0,255,0,192));
            laser.setRotation(shotDelay/2);
            laser.setPosition(sprite.getPosition() + sf::Vector2f(-18, 7));
            window.draw(laser);
        }
    }

    virtual void postRender(sf::RenderTarget& window)
    {
        sf::RectangleShape healthBarBG(sf::Vector2f(280, 10));
        healthBarBG.setFillColor(sf::Color::Transparent);
        healthBarBG.setOutlineColor(sf::Color(128, 128, 128, 128));
        healthBarBG.setOutlineThickness(1);
        healthBarBG.setPosition(20, 10);
        window.draw(healthBarBG);

        sf::RectangleShape healthBar(sf::Vector2f(280 * health / maxHealth, 10));
        healthBar.setFillColor(sf::Color(212, 0, 0, 128));
        healthBar.setPosition(20, 10);
        window.draw(healthBar);
    }

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
    {
        if (damageType >= 0)
            return false;
        if (invulnerability)
            return true;
        
        if (state != BS_FlyIn)
            health -= damageAmount;
        
        if (health <= 0)
        {
            health = 0;
            destroy();
            addScore(500);
            for(unsigned int n=0; n<20; n++)
            {
                new Explosion(sprite.getPosition() + sf::Vector2f(random(-50, 50), random(-80, 80)), 10);
            }
        }
        invulnerability = 15;
        return true;
    }
};
#endif // BRE_H
