#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "gameEntity.h"
#include "EnemyGroup.h"

class MainMenu : public GameEntity
{
    constexpr static float introTextDelay = 30.0f;
    constexpr static float intoTextSpeed = 10.0f;
    float blink;
    float introTextPosition;
    Sprite logoSprite;
public:
    P<EnemyGroup> enemyGroup;

    MainMenu();
    virtual ~MainMenu();

    virtual void update(float delta) override;

    virtual void render(sp::RenderTarget& window) override;
};

#endif//MAIN_MENU_H
