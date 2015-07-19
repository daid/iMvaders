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
    PlayerInfo playerInfo[MAX_PLAYERS];
    int stageNr;
    int playerCount;
    float startStageDelay;
    float player2SpawnDelay;
    
    float reviveDelay;
    constexpr static float reviveTimeout = 3.0;
public:
    GameState(int playerCount);
    virtual ~GameState();

    virtual void update(float delta);

    virtual void render(sf::RenderTarget& window);
};

#endif//GAME_STATE_H
