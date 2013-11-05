
class Enemy: public GameEntity
{
public:
    Curve flightCurve;
    sf::Sprite sprite;
    int shotDelay;
    static const int shotAngle = 120;
    
    Enemy()
    {
        sprite.setTexture(invaderTexture, true);
        sprite.setOrigin(invaderTexture.getSize().x/2, invaderTexture.getSize().y/2);
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
        if (flightCurve.delta < 1.0)
        {
            flightCurve.moveDistance(2.0);
            sprite.setRotation(flightCurve.angle());
        }
        else
        {
            sprite.setRotation(180);
        }
        sprite.setPosition(flightCurve.getPosition());
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
        if ((position - sprite.getPosition()) > 10.0f)
            return false;
        destroy();
        return true;
    }
};
