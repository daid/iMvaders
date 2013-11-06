
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
    
    Enemy(sf::Vector2f entryPosition, sf::Vector2f finalPosition)
    {
        state = ES_FlyIn;
        
        flyInCurve.p0 = entryPosition;
        flyInCurve.p1 = finalPosition;
        flyInCurve.cp0 = entryPosition + sf::Vector2f(0, 30);
        flyInCurve.cp1 = finalPosition - sf::Vector2f(0, 30);
        
        sprite.setTexture(invaderTexture, true);
        sprite.setOrigin(invaderTexture.getSize().x/2, invaderTexture.getSize().y/2);
        sprite.setPosition(entryPosition);
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
        diveCurve.cp1 = target + sf::Vector2f(0, -30);
        diveCurve.delta = 0.0;
        state = ES_Diving;
    }
    
    void wait()
    {
        state = ES_Wait;
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
        sf::RectangleShape debug(sf::Vector2f(1, 1));
        debug.setFillColor(sf::Color(255,255,255,64));
        for(unsigned int n=0; n<100; n++)
        {
            debug.setPosition(flightCurve.getPosition(-flightCurve.delta + 1.0f/100.0 * n));
            window.draw(debug);
        }
        debug.setFillColor(sf::Color(255,0,255,64));
        debug.setSize(sf::Vector2f(3, 3));
        debug.setPosition(flightCurve.cp0);
        window.draw(debug);
        debug.setPosition(flightCurve.cp1);
        window.draw(debug);
#endif
    }
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
    {
        if (damageType != 2)
            return false;
        if ((position - sprite.getPosition()) > 8.0f)
            return false;
        destroy();
        return true;
    }
};

class EnemyGroup : public GameEntity
{
private:
    PVector<Enemy> enemyList;
    sf::Vector2f entryPoint;
public:
    EnemyGroup(sf::Vector2f entryPoint)
    : entryPoint(entryPoint)
    {
    }
    
    void add(sf::Vector2f targetPoint)
    {
        Enemy* e = new Enemy(entryPoint, targetPoint);
        e->wait();
        enemyList.push_back(e);
    }
    
    virtual ~EnemyGroup() {}
    
    virtual void update()
    {
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
    
    void flyIn()
    {
        foreach(Enemy, e, enemyList)
            e->wait();
        foreach(Enemy, e, enemyList)
        {
            e->flyIn();
            break;
        }
    }
};
