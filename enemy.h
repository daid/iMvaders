
enum EnemyState
{
    ES_Wait,
    ES_FlyIn,
    ES_CenterField,
    ES_Diving,
    ES_Outside
};

class Enemy: public GameEntity
{
public:
    EnemyState state;
    Curve flyInCurve;
    Curve diveCurve;
    int shotDelay;
    static const int shotAngle = 120;

public:    
    sf::Sprite sprite;
    
    Enemy(sf::Vector2f finalPosition)
    {
        state = ES_Outside;
        
        flyInCurve.p0 = sf::Vector2f(-50, -50);
        flyInCurve.p1 = finalPosition;
        flyInCurve.cp0 = flyInCurve.p0;
        flyInCurve.cp1 = finalPosition - sf::Vector2f(0, 30);
        
        sprite.setTexture(invaderTexture, true);
        sprite.setOrigin(invaderTexture.getSize().x/2, invaderTexture.getSize().y/2);
        sprite.setPosition(flyInCurve.p0);
        shotDelay = random(50, 500);
    }
    virtual ~Enemy()
    {
    }
    
    virtual void update()
    {
        if (shotDelay)
        {
            shotDelay--;
        }else{
            shotDelay = random(200, 400);
            float a = sprite.getRotation();
            if (a > 180 - shotAngle/2 && a < 180 + shotAngle/2)
                new Bullet(sprite.getPosition(), 0, a);
        }
        
        switch(state)
        {
        case ES_Wait:
            break;
        case ES_FlyIn:
            if (flyInCurve.delta < 1.0)
            {
                flyInCurve.moveDistance(2.0);
                sprite.setRotation(flyInCurve.angle());
            }
            else
            {
                sprite.setRotation(180);
                state = ES_CenterField;
            }
            sprite.setPosition(flyInCurve.getPosition());
            break;
        case ES_CenterField:
            break;
        case ES_Diving:
            if (diveCurve.delta < 1.0)
            {
                diveCurve.moveDistance(2.0);
                sprite.setRotation(diveCurve.angle());
            }
            else
            {
                flyInCurve.delta = 0.0;
                state = ES_Outside;
            }
            sprite.setPosition(diveCurve.getPosition());
            break;
        case ES_Outside:
            break;
        }
    }
    
    void dive(sf::Vector2f target)
    {
        diveCurve.p0 = sprite.getPosition();
        diveCurve.cp0 = diveCurve.p0 + sf::vector2FromAngle(sprite.getRotation()) * 30.0f;
        diveCurve.p1 = target;
        diveCurve.cp1 = sf::Vector2f(target.x, 180);
        diveCurve.delta = 0.0;
        state = ES_Diving;
    }
    
    void wait(sf::Vector2f start)
    {
        state = ES_Wait;
        flyInCurve.p0 = start;
        flyInCurve.cp0 = start + sf::Vector2f(0, 30);
        sprite.setPosition(flyInCurve.p0);
    }
    void flyIn()
    {
        state = ES_FlyIn;
        flyInCurve.delta = 0.0;
    }
    
    virtual void render(sf::RenderTarget& window)
    {
        window.draw(sprite);

#ifdef DEBUG
        flyInCurve.draw(window);
        diveCurve.draw(window);
#endif
    }
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
    {
        if (damageType != 2)
            return false;
        if ((position - sprite.getPosition()) > 8.0f)
            return false;
        new Explosion(sprite.getPosition(), 8);
        destroy();
        return true;
    }
};

class EnemyGroup : public GameEntity
{
private:
    PVector<Enemy> enemyList;
public:
    EnemyGroup()
    {
    }
    
    void add(sf::Vector2f targetPoint)
    {
        Enemy* e = new Enemy(targetPoint);
        enemyList.push_back(e);
    }
    
    virtual ~EnemyGroup() {}
    
    virtual void update()
    {
        if (enemyList.size() < 1)
            destroy();
    
        P<Enemy> prev;
        foreach(Enemy, e, enemyList)
        {
            if (e->state == ES_Wait)
            {
                if (!prev || e->sprite.getPosition() - prev->sprite.getPosition() > 20.0f)
                    e->flyIn();
                break;
            }
            if (e->state == ES_CenterField && prev && prev->state == ES_Diving)
            {
                if (e->sprite.getPosition() - prev->sprite.getPosition() > 30.0f)
                    e->dive(prev->diveCurve.p1);
                break;
            }
            prev = e;
        }
    }
    
    bool isAll(EnemyState state)
    {
        foreach(Enemy, e, enemyList)
            if (e->state != state)
                return false;
        return true;
    }
    
    void dive(sf::Vector2f target)
    {
        foreach(Enemy, e, enemyList)
        {
            e->dive(target);
            break;
        }
    }
    
    void flyIn(sf::Vector2f start)
    {
        foreach(Enemy, e, enemyList)
            e->wait(start);
        foreach(Enemy, e, enemyList)
        {
            e->flyIn();
            break;
        }
    }
};
