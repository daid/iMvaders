#include "windowManager.h"
#include "background.h"
#include "MainMenu.h"

int main()
{
    WindowManager window(320, 240, false);
    new StarBackground();
    new MainMenu();
    window.mainLoop();
    return EXIT_SUCCESS;
}
