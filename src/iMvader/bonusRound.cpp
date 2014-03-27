#include "scriptInterface.h"
#include "random.h"
#include "textureManager.h"
#include "scoreManager.h"
#include "engine.h"
#include "player.h"
#include "explosion.h"
#include "stringUtils.h"
#include "transmission.h"
#include "gameEntity.h"
#include "Collisionable.h"

class BonusRoundObject;
class BonusRound : public GameEntity
{
    sf::Sprite youmagineSprite;
    sf::Sprite thingiverseSprite;
    
    float introTimeout;
    float speed;
    int spawnCount;
    float spawnTimeout;
    float spawnDelay;
    int faultCount;
    int scoreCount;
    PVector<BonusRoundObject> objects;
public:
    ScriptCallback finished;

    BonusRound()
    {
        textureManager.setTexture(youmagineSprite, "youmagine");
        textureManager.setTexture(thingiverseSprite, "thingiverse");
        textureManager.setTexture(sprite, "bonus");
        sprite.setPosition(160, 120);
        youmagineSprite.setPosition(40, 60);
        thingiverseSprite.setPosition(70, 180);
        youmagineSprite.setColor(sf::Color(255,255,255,128));
        thingiverseSprite.setColor(sf::Color(255,255,255,128));
        spawnCount = 25;
        spawnTimeout = 2.0;
        spawnDelay = 2.0;
        introTimeout = 3.0;
        speed = 35;
        faultCount = 0;
        scoreCount = 0;
        playerBonusWeaponsActive = true;
    }
    virtual ~BonusRound()
    {
        //Make sure the bonus weapons are disabled if the bonusround gets destroyed by other means then a normal end.
        playerBonusWeaponsActive = false;
    }

    virtual void update(float delta);
    
    virtual void preRender(sf::RenderTarget& window)
    {
        window.draw(youmagineSprite);
        window.draw(thingiverseSprite);
    }
    virtual void postRender(sf::RenderTarget& window)
    {
        if (introTimeout > 0.0)
            window.draw(sprite);
    }
    
    void score()
    {
        scoreCount++;
        
        if (spawnDelay > 1.7)
            spawnDelay -= 0.2;
        if (spawnDelay > 1.0)
            spawnDelay -= 0.3;
        speed += 3;
    }
    
    void fault()
    {
        faultCount ++;
        
        spawnDelay += 0.5;
        if (spawnDelay > 2.0)
            spawnDelay = 2.0;
        speed -= 5;
        if (speed < 35)
            speed = 35;
    }
};

class BonusRoundObject : public GameEntity, public Collisionable
{
    P<BonusRound> owner;
    float speed;
    float yMove;
    int type;
public:
    BonusRoundObject(P<BonusRound> owner, float speed)
    : Collisionable(15), owner(owner), speed(speed)
    {
        setPosition(sf::Vector2f(350, random(50, 190)));
        if (random(0, 100) < 50)
            type = 1;
        else
            type = 0;
        
        if (type == 1)
            textureManager.setTexture(sprite, "cool_print_1");
        else
            textureManager.setTexture(sprite, "iphone_case");
        yMove = 0;
    }
    
    virtual void update(float delta)
    {
        if (!owner)
        {
            destroy();
            return;
        }
        setPosition(getPosition() + sf::Vector2f(-speed, speed*yMove) * delta);
        yMove = 0.0;
        if (getPosition().x < 120)
        {
            if (getPosition().y < 120)
                setPosition(getPosition() + sf::Vector2f(0, -getPosition().y + 60) / 100.0f * speed * delta);
            else
                setPosition(getPosition() + sf::Vector2f(0, -getPosition().y + 180) / 100.0f * speed * delta);
        }
        if (getPosition().x < -30)
        {
            if ((type == 1 && getPosition().y > 120) || (type != 1 && getPosition().y < 120))
                owner->fault();
            else if (type == 1)
                owner->score();
            destroy();
        }
    }
    
    virtual void render(sf::RenderTarget& window)
    {
        sprite.setPosition(getPosition());
        window.draw(sprite);
    }
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
    {
        if (damageType == -1)
            yMove +=-1.0f;
        else
            yMove += 1.0f;
        return false;
    }
};

REGISTER_SCRIPT_CLASS(BonusRound)
{
    REGISTER_SCRIPT_CLASS_CALLBACK(BonusRound, finished);
}

void BonusRound::update(float delta)
{
    if (introTimeout > 0.0)
    {
        introTimeout -= delta;
        if (introTimeout > 2.0)
            sprite.setRotation(introTimeout * 360 * 5);
        else
            sprite.setRotation(0);
    }
    if (spawnCount > 0)
    {
        if (spawnTimeout > 0.0)
        {
            spawnTimeout -= delta;
        }else{
            spawnTimeout += spawnDelay;
            objects.push_back(new BonusRoundObject(this, speed));
            spawnCount--;
        }
    }else{
        objects.update();
        if (objects.size() < 1)
        {
            P<Transmission> t = new Transmission();
            t->setFace("Henk1");
            t->setText(("Scored " + to_string(scoreCount * 32) + "|bonus points").c_str());
            t->top();
            
            P<ScoreManager>(engine->getObject("score"))->add(scoreCount * 32);
            finished();
            destroy();
            playerBonusWeaponsActive = false;
        }
    }
}
