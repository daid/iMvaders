#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "gameEntity.h"
#include "EnemyGroup.h"

class MainMenu : public GameEntity
{
    const static float introTextDelay = 30.0f;
    const static float intoTextSpeed = 10.0f;
    float blink;
    float introTextPosition;
public:
    sf::Sprite logoSprite;
    P<EnemyGroup> enemyGroup;

    MainMenu();

    virtual ~MainMenu();

    virtual void update(float delta);

    virtual void postRender(sf::RenderTarget& window);
};

#endif//MAIN_MENU_H
