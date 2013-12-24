#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "gameEntity.h"
#include "scriptInterface.h"
#include "player.h"

class GameState : public GameEntity
{
private:
    P<PlayerCraft> player[MAX_PLAYERS];
    P<ScriptObject> script;
    int lives[MAX_PLAYERS];
    int stageNr;
    int playerCount;
    float startStageDelay;
public:
    GameState(int playerCount);
    virtual ~GameState();

    virtual void update(float delta);

    virtual void postRender(sf::RenderTarget& window);
};

#endif//GAME_STATE_H
