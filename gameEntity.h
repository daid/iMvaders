#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include "P.h"

/**
    GameEntities are the base for each object in the game.
    They are automaticly added to the entityList, which means they get update() and render() calls.
    
    The GameEntities are based of the Pobject class, which helps in tracking references and destruction.
    
    Create a new GameEntity with "new GameEntity()" and destroy it with "entity->destroy()", never use
    "delete entity", as the Pobject takes care of this.
 */

class GameEntity;
PVector<GameEntity> entityList;
class GameEntity: public Pobject
{
public:
    GameEntity()
    {
        entityList.push_back(this);
        //printf("Created: %p\n", this);
    }
    
    virtual ~GameEntity()
    {
        //printf("Destroyed: %p\n", this);
    }
    
    virtual void update() {}
    
    virtual void preRender(sf::RenderTarget& window) {}
    virtual void render(sf::RenderTarget& window) {}
    virtual void postRender(sf::RenderTarget& window) {}
    
    virtual bool takeDamage(sf::Vector2f position, int damageType, int damageAmount) { return false; }
};

#endif//GAME_ENTITY_H
