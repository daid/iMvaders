#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include <SFML/Graphics.hpp>
#include "P.h"

/**
    GameEntities are the base for each object in the game.
    They are automaticly added to the entityList, which means they get update() and render() calls.

    The GameEntities are based of the Pobject class, which helps in tracking references and destruction.

    Create a new GameEntity with "new GameEntity()" and destroy it with "entity->destroy()", never use
    "delete entity", as the PObject takes care of this.
 */

class GameEntity;
extern PVector<GameEntity> entityList;
class GameEntity: public PObject
{
public:
    sf::Sprite sprite;
    float collisionRadius;

    GameEntity(float collisionRadius = 0.0f)
    : collisionRadius(collisionRadius)
    {
        entityList.push_back(this);
        //printf("Created: %p\n", this);
    }

    virtual ~GameEntity();

    virtual void update();

    virtual void preRender(sf::RenderTarget& window);
    virtual void render(sf::RenderTarget& window);
    virtual void postRender(sf::RenderTarget& window);

    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount);
};

#define foreach_hit(var, source) \
    foreach(GameEntity, var, entityList) \
        if (var->collisionRadius > 0.0 && *var != source && (var->sprite.getPosition() - source->sprite.getPosition()) <= var->collisionRadius + source->collisionRadius)

#endif//GAME_ENTITY_H
