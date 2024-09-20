#include <math.h>

#include "engine.h"
#include "scriptInterface.h"
#include "random.h"
#include "textureManager.h"
#include "scoreManager.h"
#include "engine.h"
#include "explosion.h"

class SurfHenk : public GameEntity
{
    static constexpr float speed = 80.0f;
public:
    SurfHenk()
    {
        spriteManager.setTexture(sprite, "henk-surf");
        sprite.setPosition({-100, 0});
    }
    virtual ~SurfHenk() {}

    virtual void update(float delta) override
    {
        sprite.setPosition({sprite.getPosition().x + delta * speed, sin(sprite.getPosition().x / 25) * 20 + 150});
        if (sprite.getPosition().x > 400)
            destroy();
    }

    virtual void render(sp::RenderTarget& window) override
    {
        sprite.draw(window);
    }
};

REGISTER_SCRIPT_CLASS(SurfHenk)
{
}
