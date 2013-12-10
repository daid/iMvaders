#include "mainMenu.h"
#include "player.h"
#include "gameState.h"
#include "textDraw.h"
#include "scoreManager.h"

MainMenu::MainMenu()
{
    blink = 0;
    textureManager.setTexture(logoSprite, "iMvader");
    logoSprite.setPosition(160, 60);

    enemyGroup = new EnemyGroup();
    for(unsigned int n=0; n<10; n++)
    {
        BasicEnemyBase* e = new BasicEnemy();
        e->setTargetPosition(sf::Vector2f(160+4*20 - n * 20, 100));
        enemyGroup->add(e);
    }

    sf::Listener::setGlobalVolume(0);
}

MainMenu::~MainMenu()
{
}

void MainMenu::update(float delta)
{
    blink += delta;
    if (enemyGroup->isAll(ES_CenterField))
        enemyGroup->dive(sf::Vector2f(random(20, 300), 260));
    if (enemyGroup->isAll(ES_Outside))
        enemyGroup->flyIn(sf::Vector2f(random(0, 320), -20));

    if (playerController[0].fire())
    {
        new GameState(1);
        sf::Listener::setGlobalVolume(100);
    }
    else if (playerController[1].fire())
    {
        new GameState(2);
        sf::Listener::setGlobalVolume(100);
    }
}

void MainMenu::postRender(sf::RenderTarget& window)
{
    window.draw(logoSprite);
    drawText(window, 160, 120, "HIGH SCORE");
    for(int i=0; i<score.highscoreListSize; i++)
    {
        drawText(window, 160, 120 + 16 * (i+1), score.getHighscoreName(i) + " " + to_string(score.getHighScore(i)));
    }
    if (int(blink * 1000) % 1000 < 500)
        drawText(window, 160, 200, "INSERT COIN");
}
