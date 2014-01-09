#include "mainMenu.h"
#include "player.h"
#include "gameState.h"
#include "textDraw.h"
#include "stringUtils.h"
#include "scoreManager.h"
#include "engine.h"

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
    introTextPosition = 0.0;
}

MainMenu::~MainMenu()
{
}

void MainMenu::update(float delta)
{
    blink += delta;
    introTextPosition += delta;
    if (enemyGroup->isAll(ES_CenterField))
        enemyGroup->dive(sf::Vector2f(random(20, 300), 260));
    if (introTextPosition < introTextDelay)
    {
        if (enemyGroup->isAll(ES_Outside))
            enemyGroup->flyIn(sf::Vector2f(random(0, 320), -20));
    }

    P<PlayerController> pc1 = engine->getObject("playerController1");
    P<PlayerController> pc2 = engine->getObject("playerController2");
    if (pc1->button(fireButton))
    {
        new GameState(1);
        sf::Listener::setGlobalVolume(100);
    }
    else if (pc2->button(fireButton))
    {
        new GameState(2);
        sf::Listener::setGlobalVolume(100);
    }
}

void MainMenu::postRender(sf::RenderTarget& window)
{
    P<ScoreManager> score = engine->getObject("score");
    if (introTextPosition < introTextDelay)
    {
        drawText(window, 160, 120, "HIGH SCORE");
        for(int i=0; i<ScoreManager::highscoreListSize; i++)
        {
            drawText(window, 160, 120 + 16 * (i+1), score->getHighscoreName(i) + " " + to_string(score->getHighScore(i)));
        }
        if (int(blink * 1000) % 1000 < 500)
            drawText(window, 160, 200, "INSERT COIN");
    }else{
        float f = (introTextPosition - introTextDelay) * intoTextSpeed;
        drawText(window, 160, 250 - f, "THE YEAR IS 2022"); f -= 15.0f;
        drawText(window, 160, 250 - f, "THIS IS THE FUTURE"); f -= 15.0f;
        f -= 10.0f;
        drawText(window, 160, 250 - f, "IN 2015 THE M CORPERATION"); f -= 15.0f;
        drawText(window, 160, 250 - f, "LAUNCHED THEIR FIRST 3D PRINTER"); f -= 15.0f;
        drawText(window, 160, 250 - f, "INTO SPACE"); f -= 15.0f;
        f -= 10.0f;
        drawText(window, 160, 250 - f, "APPLAUDED BY THE WHOLE WORLD"); f -= 15.0f;
        drawText(window, 160, 250 - f, "AS A TECHNICAL STEP FORWARDS"); f -= 15.0f;
        drawText(window, 160, 250 - f, "TOWARDS SPACE EXPLORATION"); f -= 15.0f;
        drawText(window, 160, 250 - f, "AND WORLD PEACE"); f -= 15.0f;
        f -= 10.0f;
        drawText(window, 160, 250 - f, "BUT UNKNOWN TO THE WORLD"); f -= 15.0f;
        drawText(window, 160, 250 - f, "M CORPERATION HAD OTHER PLANS"); f -= 15.0f;
        f -= 10.0f;
        drawText(window, 160, 250 - f, "BY 2020 THE M CORPERATION"); f -= 15.0f;
        drawText(window, 160, 250 - f, "PLACED THE FIRST GIANT"); f -= 15.0f;
        drawText(window, 160, 250 - f, "AUTOMATED MINING 3D PRINTER"); f -= 15.0f;
        drawText(window, 160, 250 - f, "ON THE MOON"); f -= 15.0f;
        f -= 10.0f;
        drawText(window, 160, 250 - f, "WITHIN A YEAR THIS MOON"); f -= 15.0f;
        drawText(window, 160, 250 - f, "PRINTER PRODUCED COUNTLESS"); f -= 15.0f;
        drawText(window, 160, 250 - f, "SMALL VERSIONS OF ITSELF"); f -= 15.0f;
        drawText(window, 160, 250 - f, "THAT GOT SEND OUT TO THE"); f -= 15.0f;
        drawText(window, 160, 250 - f, "ASTROID BELT"); f -= 15.0f;
        f -= 10.0f;
        drawText(window, 160, 250 - f, "THE ASTROID BASED PRINTERS"); f -= 15.0f;
        drawText(window, 160, 250 - f, "QUICKLY USED THE ASTROIDS TO"); f -= 15.0f;
        drawText(window, 160, 250 - f, "PRODUCE AN ARMY OF SPACESHIPS"); f -= 15.0f;
        f -= 10.0f;
        drawText(window, 160, 250 - f, "NOW THERE IS ONLY ONCE FORCE"); f -= 15.0f;
        drawText(window, 160, 250 - f, "THAT HAS A CHANCE OF STOPPING"); f -= 15.0f;
        drawText(window, 160, 250 - f, "THE M CORPERATIONS QUEST FOR"); f -= 15.0f;
        drawText(window, 160, 250 - f, "GALACTIC DOMINANCE"); f -= 15.0f;
        f -= 10.0f;
        drawText(window, 160, 250 - f, "THIS FORCE IS THE"); f -= 15.0f;
        drawText(window, 160, 250 - f, "U FORCE"); f -= 15.0f;
        f -= 50.0f;
        drawText(window, 160, 250 - f, "DUM DUM DUMMMMMM"); f -= 15.0f;
        f -= 50.0f;
        drawText(window, 160, 250 - f, "OH AND"); f -= 15.0f;
        drawText(window, 160, 250 - f, "GOOD LUCK"); f -= 15.0f;
        f -= 100.0f;
        drawText(window, 160, 250 - f, "ALSO"); f -= 15.0f;
        drawText(window, 160, 250 - f, "WE HAVE COOKIES"); f -= 15.0f;
        
        if (f > 250)
            introTextPosition = 0;
    }
    window.draw(logoSprite);
}
