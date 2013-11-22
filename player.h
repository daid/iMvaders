
class PlayerController: public sf::NonCopyable
{
public:
    sf::Keyboard::Key leftKey;
    sf::Keyboard::Key rightKey;
    
    PlayerController()
    : leftKey(sf::Keyboard::Left), rightKey(sf::Keyboard::Right)
    {
    }

    bool left() { return sf::Keyboard::isKeyPressed(leftKey); }
    bool right() { return sf::Keyboard::isKeyPressed(rightKey); }
    bool up() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Up); }
    bool down() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Down); }
    bool fire() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Space); }
};

class PlayerCraft: public GameEntity
{
public:
    PlayerController* controller;
    sf::Vector2f velocity;
    float fireCooldown;
    float invulnerability;
public:
    PlayerCraft(PlayerController* controller)
    : GameEntity(10.0f), controller(controller)
    {
        invulnerability = 1.0;
        fireCooldown = 0.4;
        textureManager.setTexture(sprite, "m");
        sprite.setPosition(sf::Vector2f(160, 220));
    }

    virtual ~PlayerCraft()
    {
    }

    virtual void update(float delta)
    {
        if (fireCooldown > 0)
            fireCooldown -= delta;
        if (invulnerability > 0)
            invulnerability -= delta;

        velocity = velocity * 0.85f;//TODO: Proper dampening with use of delta.
        if (controller->left())
            velocity = sf::Vector2f(-100.0, velocity.y);
        if (controller->right())
            velocity = sf::Vector2f( 100.0, velocity.y);
        if (controller->up())
            velocity = sf::Vector2f(velocity.x, -100);
        if (controller->down())
            velocity = sf::Vector2f(velocity.x,  100);
        
        sprite.setPosition(sprite.getPosition() + velocity * delta);

        if (sprite.getPosition().x < 20)
            sprite.setPosition(20, sprite.getPosition().y);
        if (sprite.getPosition().x > 300)
            sprite.setPosition(300, sprite.getPosition().y);
        if (sprite.getPosition().y < 10)
            sprite.setPosition(sprite.getPosition().x, 10);
        if (sprite.getPosition().y > 230)
            sprite.setPosition(sprite.getPosition().x, 230);

        if (controller->fire() && fireCooldown <= 0 && invulnerability <= 0)
        {
            new Bullet(sprite.getPosition(), -1, 0);
            fireCooldown = 0.4;
        }
        if (!controller->fire() && fireCooldown > 0.1)
            fireCooldown = 0.1;
    }

    virtual void render(sf::RenderTarget& window)
    {
        if (fmod(invulnerability, 4.0/60.0) > 2.0/60.0)
            return;
        sprite.setRotation(velocity.x / 10.0);
        window.draw(sprite);
    }

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount)
    {
        if (damageType < 0 || invulnerability > 0)
            return false;
        destroy();
        new Explosion(sprite.getPosition(), 12);
        return true;
    }
};
