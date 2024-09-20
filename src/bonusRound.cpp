#include "scriptInterface.h"
#include "random.h"
#include "textureManager.h"
#include "soundManager.h"
#include "scoreManager.h"
#include "engine.h"
#include "player.h"
#include "explosion.h"
#include "transmission.h"
#include "gameEntity.h"
#include "main.h"

class BonusRoundRow;
class BonusRound : public GameEntity
{
    float introTimeout;
    int spawnCount;
    float spawnTimeout;
    float spawnDelay;
    int scoreCount;
    int colorCount;
    PVector<BonusRoundRow> objects;
    P<Transmission> endTransmission;
public:
    ScriptCallback finished;

    BonusRound();
    
    virtual ~BonusRound()
    {
        //Make sure the bonus weapons are disabled if the bonusround gets destroyed by other means then a normal end.
        playerBonusWeaponsActive = false;
    }

    virtual void update(float delta) override;
    
    virtual void render(sp::RenderTarget& window) override
    {
        if (introTimeout > 0.0f)
            sprite.draw(window);
    }
    
    void score()
    {
        scoreCount++;
        soundManager->playSound("bonus_score.wav", random(0.95, 1.05));
    }
    
    void setColorCount(int count)
    {
        colorCount = count;
    }
};

class BonusRoundObject : public GameEntity, public Collisionable
{
    P<BonusRound> owner;
    float switchDelay;
    bool scored;
    int colorCount;
    int disallowColor;
    static const int maxColorCount = 7;
public:
    int type;

    BonusRoundObject(P<BonusRound> owner, int idx, int colorCount, int disallowColor)
    : Collisionable(glm::vec2(20, 20)), owner(owner), disallowColor(disallowColor)
    {
        setPosition(glm::vec2(70 + 20 * idx, -10));
        setVelocity(glm::vec2(0, 10.0));
        
        if (colorCount > maxColorCount)
            colorCount = maxColorCount;
        
        this->colorCount = colorCount;
        
        spriteManager.setTexture(sprite, "robot");
        sprite.setScale(0.25, 0.25);
        do {
            type = irandom(0, colorCount - 1);
        } while(type == disallowColor);
        
        switchDelay = 0.0;
        scored = false;
    }
    
    virtual void update(float delta) override
    {
        if (switchDelay > 0)
            switchDelay -= delta;
        if (!owner)
        {
            destroy();
            return;
        }
        if (getPosition().y > 260)
            destroy();
        if (scored)
        {
            sprite.setScale(sprite.getScale() * powf(0.1, delta));
            if (sprite.getScale().x < 0.01f)
                destroy();
        }
    }
    
    virtual void render(sp::RenderTarget& window) override
    {
        switch(type)
        {
        case 0: sprite.setColor({255,  0,  0,255}); break;
        case 1: sprite.setColor({  0,255,  0,255}); break;
        case 2: sprite.setColor({  0,  0,255,255}); break;
        case 3: sprite.setColor({255,255,  0,255}); break;
        case 4: sprite.setColor({  0,255,255,255}); break;
        case 5: sprite.setColor({255,  0,255,255}); break;
        case 6: sprite.setColor({255,255,255,255}); break;
        }
        
        sprite.setPosition(getPosition());
        sprite.setRotation(getRotation());
        sprite.draw(window);
    }
    
    virtual bool takeDamage(glm::vec2 position, int damageType, int damageAmount) override
    {
        if (scored)
            return false;
        if (damageAmount)
        {
            if (switchDelay <= 0)
            {
                do {
                    type = (type + 1) % colorCount;
                } while(type == disallowColor);
            }
            switchDelay = 0.1;
        }
        return true;
    }
    
    void score()
    {
        scored = true;
    }
};

class BonusRoundRow : public GameEntity
{
    PVector<BonusRoundObject> items;
    P<BonusRound> owner;
public:
    BonusRoundRow(P<BonusRound> owner, int colorCount)
    : owner(owner)
    {
        int disallowColor = irandom(0, colorCount - 1);
        if (colorCount < 5)
            disallowColor = -1;
        for(int n=0; n<10; n++)
        {
            items.push_back(new BonusRoundObject(owner, n, colorCount, disallowColor));
        }
    }
    virtual ~BonusRoundRow() {}
    
    virtual void update(float delta) override
    {
        int type = -1;
        foreach(BonusRoundObject, obj, items)
        {
            if (type == -1)
                type = obj->type;
            if (obj->type != type)
                return;
        }
        if (items.size() > 0)
        {
            foreach(BonusRoundObject, obj, items)
                obj->score();
            owner->score();
        }
        destroy();
    }
};

REGISTER_SCRIPT_CLASS(BonusRound)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(BonusRound, setColorCount);
    REGISTER_SCRIPT_CLASS_CALLBACK(BonusRound, finished);
}

BonusRound::BonusRound()
: GameEntity(hudLayer)
{
    spriteManager.setTexture(sprite, "bonus");
    sprite.setPosition(160, 120);
    spawnCount = 10;
    spawnTimeout = 2.0;
    spawnDelay = 2.0;
    introTimeout = 3.0;
    scoreCount = 0;
    colorCount = 4;
    playerBonusWeaponsActive = true;
}

void BonusRound::update(float delta)
{
    if (introTimeout > 0.0f)
    {
        introTimeout -= delta;
        if (introTimeout > 2.0f)
            sprite.setRotation(introTimeout * 360 * 5);
        else
            sprite.setRotation(0);
    }
    if (spawnCount > 0)
    {
        if (spawnTimeout > 0.0f)
        {
            spawnTimeout -= delta;
        }else{
            spawnTimeout += spawnDelay;
            objects.push_back(new BonusRoundRow(this, colorCount));
            spawnCount--;
        }
    }else{
        objects.update();
        if (objects.size() < 1 && !endTransmission)
        {
            if (scoreCount > 0)
            {
                int points = scoreCount * 120;
                endTransmission = new Transmission();
                endTransmission->setFace("Henk1");
                endTransmission->setText(("Scored " + string(points) + "|bonus points").c_str());
                endTransmission->top();
                
                P<ScoreManager>(engine->getObject("score"))->add(points);
                scoreCount = 0;
            }else{
                finished();
                destroy();
                playerBonusWeaponsActive = false;
            }
        }
    }
}
