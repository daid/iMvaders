#include "windowManager.h"
#include "background.h"
#include "MainMenu.h"
#include "player.h"

#include "scriptInterface.h"

int main()
{
    playerController[1].keyBind[0] = sf::Keyboard::A;
    playerController[1].keyBind[1] = sf::Keyboard::D;
    playerController[1].keyBind[2] = sf::Keyboard::W;
    playerController[1].keyBind[3] = sf::Keyboard::S;
    playerController[1].keyBind[4] = sf::Keyboard::Q;
    
    WindowManager window(320, 240, false);
    new StarBackground();
    new MainMenu();
    window.mainLoop();
    return 0;
}
