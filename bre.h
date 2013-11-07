
class BreEnemy: public GameEntity
{
public:
    sf::Sprite mouth;
    int state;
    float mouthPos;
    
    BreEnemy()
    {
        sprite.setTexture(bre1Texture, true);
        sprite.setOrigin(bre1Texture.getSize().x/2, bre1Texture.getSize().y/2);
        sprite.setPosition(sf::Vector2f(160, -80));
        mouth.setTexture(bre2Texture, true);
        mouth.setOrigin(bre2Texture.getSize().x/2, bre2Texture.getSize().y/2);
        mouth.setPosition(sf::Vector2f(160, -80));
        state = 0;
        mouthPos = 0;
    }
    virtual ~BreEnemy()
    {
    }
    
    virtual void update()
    {
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
        window.draw(sprite);
        window.draw(mouth);
        
        sf::RectangleShape laser(sf::Vector2f(6, 240));
        laser.setOrigin(3, 0);
        laser.setFillColor(sf::Color(255,0,0,192));
        laser.setRotation(mouthPos - 15);
        laser.setPosition(sprite.getPosition() + sf::Vector2f(18, 7));
        window.draw(laser);

        laser.setFillColor(sf::Color(0,255,0,192));
        laser.setPosition(sprite.getPosition() + sf::Vector2f(-18, 7));
        window.draw(laser);
    }
};
