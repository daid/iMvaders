#ifndef VERSUS_GAME_MODE_H
#define VERSUS_GAME_MODE_H

#include "gameState.h"

class VersusGameState : public GameEntity
{
private:
    P<PlayerCraft> player[MAX_PLAYERS];
    PlayerInfo playerInfo[MAX_PLAYERS];
    float printerSpawnDelay;
    constexpr static float printerSpawnTime = 1.2;
    float victoryDelay;
public:
    VersusGameState();
    virtual ~VersusGameState() {}

    virtual void update(float delta) override;

    virtual void render(sp::RenderTarget& window) override;
};

#endif//VERSUS_GAME_MODE_H
