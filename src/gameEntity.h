#pragma once

#include "Updatable.h"
#include "Renderable.h"
#include "spriteManager.h"


class GameEntity;
extern PVector<GameEntity> entityList;
class GameEntity: public Updatable, public Renderable
{
public:
    Sprite sprite;

    GameEntity()
    {
        entityList.push_back(this);
    }
    GameEntity(RenderLayer* renderLayer)
    : Renderable(renderLayer)
    {
        entityList.push_back(this);
    }

    virtual ~GameEntity();

    virtual void update(float delta) override;

    virtual void render(sp::RenderTarget& window) override;

    virtual bool takeDamage(glm::vec2 position, int damageType, int damage_amount);
};
