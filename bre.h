
class BreEnemy: public GameEntity
{
public:
    sf::Sprite mouth;
    int state;
    int health;
    float mouthPos;
    int invulnerability;
    static const int maxHealth = 100;
    
    BreEnemy()
    : GameEntity(50)
    {
        sprite.setTexture(bre1Texture, true);
        sprite.setOrigin(bre1Texture.getSize().x/2, bre1Texture.getSize().y/2);
        sprite.setPosition(sf::Vector2f(160, -80));
        mouth.setTexture(bre2Texture, true);
        mouth.setOrigin(bre2Texture.getSize().x/2, bre2Texture.getSize().y/2);
        mouth.setPosition(sf::Vector2f(160, -80));
        state = 0;
        mouthPos = 0;
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
        //new Bullet(sprite.getPosition() + sf::Vector2f(18, 7), 1, 180);
        //new Bullet(sprite.getPosition() + sf::Vector2f(-18, 7), 0, 180);
        switch(state)
        {
        case 0:
            if (sprite.getPosition().y < 80.0)
                sprite.setPosition(sprite.getPosition() + sf::Vector2f(0, 1));
            else
                state = 1;
            break;
        case 1:
            if (sprite.getPosition().x < 280.0)
                sprite.setPosition(sprite.getPosition() + sf::Vector2f(1, 0));
            else
                state = 2;
            break;
        case 2:
            if (sprite.getPosition().x > 40.0)
                sprite.setPosition(sprite.getPosition() + sf::Vector2f(-1, 0));
            else
                state = 1;
            break;
        }
        mouthPos = sprite.getPosition().x - 40;
        while(mouthPos > 60.0) mouthPos -= 60;
        if (mouthPos > 30) mouthPos = 60 - mouthPos;
        mouth.setPosition(sprite.getPosition() + sf::Vector2f(0, mouthPos));
    }
    
    virtual void render(sf::RenderTarget& window)
    {
        if (invulnerability & 2)
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
        
        /*
        sf::RectangleShape laser(sf::Vector2f(6, 240));
        laser.setOrigin(3, 0);
        laser.setFillColor(sf::Color(255,0,0,192));
        laser.setRotation(mouthPos - 15);
        laser.setPosition(sprite.getPosition() + sf::Vector2f(18, 7));
        window.draw(laser);

        laser.setFillColor(sf::Color(0,255,0,192));
        laser.setPosition(sprite.getPosition() + sf::Vector2f(-18, 7));
        window.draw(laser);
        */
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
