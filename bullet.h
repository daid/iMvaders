
class Bullet: public GameEntity
{
private:
    float speed;
    int type;
public:
    Bullet(sf::Vector2f position, int type, float angle, float speed = 3.0f)
    : GameEntity(1.0f), speed(speed), type(type)
    {
        sprite.setTexture(bulletTexture);
        sprite.setOrigin(bulletTexture.getSize().x/2, bulletTexture.getSize().y/10);
        sprite.setPosition(position);
        sprite.setRotation(angle);
        if (type == 0)
            sprite.setColor(sf::Color::Red);
        else if (type == 1)
            sprite.setColor(sf::Color::Green);
        else if (type == -1)
            sprite.setColor(sf::Color(24, 161, 212));
    }
    
    virtual void update()
    {
        sprite.setPosition(sprite.getPosition() + sf::vector2FromAngle(sprite.getRotation()) * speed);
        if (sprite.getPosition().x < -10) destroy();
        if (sprite.getPosition().y < -10) destroy();
        if (sprite.getPosition().x > 330) destroy();
        if (sprite.getPosition().y > 250) destroy();

        foreach_hit(e, this)
        {
            if (e->takeDamage(sprite.getPosition(), type, 1))
            {
                destroy();
                new Explosion(sprite.getPosition(), 3);
            }
        }
    }
    
    virtual void render(sf::RenderTarget& window)
    {
        window.draw(sprite);
    }
};
