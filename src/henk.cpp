#include <math.h>

#include "engine.h"
#include "random.h"
#include "textureManager.h"
#include "scoreManager.h"
#include "engine.h"
#include "explosion.h"

class SurfHenk : public GameEntity
{
    static const float speed = 80;
public:
    SurfHenk()
    {
        textureManager.setTexture(sprite, "henk-surf");
        sprite.setPosition(sf::Vector2f(-100, 0));
    }
    virtual ~SurfHenk() {}

    virtual void update(float delta)
    {
        sprite.setPosition(sf::Vector2f(sprite.getPosition().x + delta * speed, sin(sprite.getPosition().x / 25) * 20 + 150));
        if (sprite.getPosition().x > 400)
            destroy();
    }
    
    virtual void render(sf::RenderTarget& window)
    {
        window.draw(sprite);
    }
};

REGISTER_SCRIPT_CLASS(SurfHenk)
{
}
