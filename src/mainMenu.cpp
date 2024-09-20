#include "mainMenu.h"
#include "player.h"
#include "gameState.h"
#include "textDraw.h"
#include "scoreManager.h"
#include "engine.h"
#include "event.h"
#include "soundManager.h"
#include "versusGameMode.h"
#include "main.h"
#include "io/network/address.h"

class SecretMenuOption
{
public:
    string name;
    string command;
};

class SecretMenu : public Updatable, public Renderable
{
    int selectionIndex;
    float delay;
    float timeout;
    std::vector<SecretMenuOption> options;
public:
    SecretMenu()
    : Renderable(hudLayer)
    {
        selectionIndex = 0;
        delay = 0;
        timeout = 10 * 60;
        eventManager.fire("glitch");
        
        P<ResourceStream> s = getResourceStream("secretoptions.txt");
        if (!s)
            return;
        for(string line = s->readLine(); line.length() > 0; line = s->readLine())
        {
            std::vector<string> parts = line.split(":", 1);
            if (parts.size() == 2)
            {
                options.push_back(SecretMenuOption());
                options[options.size()-1].name = parts[0];
                options[options.size()-1].command = parts[1];
            }
        }
    }

    void update(float delta) override
    {
        P<PlayerController> pc1 = engine->getObject("playerController1");
        timeout -= delta;
        
        if (pc1->button(fireButton) || timeout < 0)
        {
            if (timeout < 0)
                selectionIndex = 0;
            switch(selectionIndex)
            {
            case 0:
                destroy();
                new MainMenu();
                break;
            case 1:
                destroy();
                soundManager->setMasterSoundVolume(100);
                new VersusGameState();
                break;
            default:
                {
                    SecretMenuOption option = options[selectionIndex-2];
                    FILE* f = fopen("run_after.sh", "w");
                    fprintf(f, "#!/bin/sh\n%s\n", option.command.c_str());
                    fclose(f);
                    engine->shutdown();
                }
                break;
            }
        }
        
        if (delay > 0)
        {
            delay -= delta;
        }else{
            if (pc1->down())
            {
                selectionIndex++;
                delay = 0.2;
            }
            if (pc1->up())
            {
                selectionIndex--;
                delay = 0.2;
            }
        }
        selectionIndex = std::max(0, selectionIndex);
        selectionIndex = std::min(int(options.size()) + 1, selectionIndex);
    }

    void render(sp::RenderTarget& window) override
    {
        drawText(window, 160, 20, "Choose your destiny");

        RectangleShape selection({320, 12});
        selection.setFillColor({24, 161, 212, 255});
        selection.setPosition({0, 50 + 20 * selectionIndex});
        selection.draw(window);

        int n=0;
        drawText(window, 160, 50 + 20 * n, "Play iMvaders");n++;
        drawText(window, 160, 50 + 20 * n, "Play VS iMvaders");n++;
        for(unsigned int i=0; i<options.size(); i++)
        {
            drawText(window, 160, 50 + 20 * n, options[i].name);n++;
        }

        auto localIp = sp::io::network::Address::getLocalAddress();
        drawText(window, 30, 220, localIp.getHumanReadable()[0], align_left, 0.5);
    }
};

MainMenu::MainMenu()
: GameEntity(hudLayer)
{
    blink = 0;
    spriteManager.setTexture(logoSprite, "iMvader");
    logoSprite.setPosition(160, 30);

    enemyGroup = new EnemyGroup();
    for(unsigned int n=0; n<10; n++)
    {
        BasicEnemyBase* e = new BasicEnemy();
        e->setTargetPosition({160+4*20 - n * 20, 70});
        enemyGroup->add(e);
    }

    soundManager->setMasterSoundVolume(0);
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
        enemyGroup->dive({random(20, 300), 260});
    if (introTextPosition < introTextDelay)
    {
        if (enemyGroup->isAll(ES_Outside))
            enemyGroup->flyIn({random(0, 320), -20});
    }

    P<PlayerController> pc1 = engine->getObject("playerController1");
    P<PlayerController> pc2 = engine->getObject("playerController2");
    if (pc1->down() || pc2->down())
        introTextPosition += delta * 5;
    if (blink > 1.0f)
    {
        for(int n=0; n<MAX_PLAYERS; n++)
        {
            P<PlayerController> pc = engine->getObject("playerController" + string(n + 1));
            if (pc->button(fireButton))
            {
                new GameState(n + 1);
                soundManager->setMasterSoundVolume(100);
                break;
            }
        }
    }

    if (escape_key.getDown())
    {
        engine->shutdown();
    }
    
    //Magic 4 button exit combo, to quit&rebuild on the arcade machine.
    if (pc1->button(nukeButton) && pc1->button(skipButton) && pc2->button(nukeButton) && pc2->button(skipButton))
    {
        engine->shutdown();
    }

    //Magic 4 button menu combo
    if ((pc1->button(slowButton) && pc1->button(chargeShotButton) && pc2->button(slowButton) && pc2->button(chargeShotButton)))
    {
        foreach(GameEntity, e, entityList)
            e->destroy();
        destroy();
        
        new SecretMenu();
    }
}

void MainMenu::render(sp::RenderTarget& window)
{
    P<ScoreManager> score = engine->getObject("score");
    if (introTextPosition < introTextDelay)
    {
        drawText(window, 160, 70, "HIGH SCORE");

        drawText(window, 80, 90, "1 PLAYER");
        for(int i=0; i<ScoreManager::highscoreListSize; i++)
            drawText(window, 80, 90 + 16 * (i+1), score->getHighscoreName(1, i) + " " + string(score->getHighScore(1, i)));
        drawText(window, 160+80, 90, "2 PLAYERS");
        for(int i=0; i<ScoreManager::highscoreListSize; i++)
            drawText(window, 160+80, 90 + 16 * (i+1), score->getHighscoreName(2, i) + " " + string(score->getHighScore(2, i)));

        if (int(blink * 1000) % 1000 < 500)
            drawText(window, 160, 210, "INSERT COOKIE");
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
    logoSprite.draw(window);
}
