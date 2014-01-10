#include "engine.h"
#include "windowManager.h"
#include "StarBackground.h"
#include "mainMenu.h"
#include "player.h"
#include "scoreManager.h"

#include "scriptInterface.h"

int main()
{
    new Engine();
    
    P<PlayerController> pc1 = new PlayerController();
    P<PlayerController> pc2 = new PlayerController();
    engine->registerObject("playerController1", pc1);
    engine->registerObject("playerController2", pc2);
    engine->registerObject("score", new ScoreManager());
    
    pc2->keyBind[0] = sf::Keyboard::A;
    pc2->keyBind[1] = sf::Keyboard::D;
    pc2->keyBind[2] = sf::Keyboard::W;
    pc2->keyBind[3] = sf::Keyboard::S;
    pc2->keyBind[4] = sf::Keyboard::Q;
    pc2->keyBind[5] = sf::Keyboard::E;
    pc2->keyBind[6] = sf::Keyboard::R;
    pc2->keyBind[7] = sf::Keyboard::F;
    pc2->keyBind[8] = sf::Keyboard::T;
    pc2->keyBind[9] = sf::Keyboard::G;
    
    engine->registerObject("windowManager", new WindowManager(320, 240, true));
    
    new StarBackground();
    new MainMenu();
    engine->runMainLoop();
    
    delete engine;
    return 0;
}
 
