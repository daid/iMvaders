#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "gameEntity.h"
#include "EnemyGroup.h"

class MainMenu : public GameEntity
{
    float blink;
public:
    sf::Sprite logoSprite;
    P<EnemyGroup> enemyGroup;

    MainMenu();

    virtual ~MainMenu();

    virtual void update(float delta);

    virtual void postRender(sf::RenderTarget& window);
};

#endif//MAIN_MENU_H
