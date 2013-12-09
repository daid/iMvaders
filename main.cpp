#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>

sf::Clock Clock;

#include "vectorUtils.h"
#include "textDraw.h"
#include "curve.h"
#include "background.h"
#include "gameEntity.h"
#include "textureManager.h"
#include "scoreManager.h"
#include "postProcessManager.h"
#include "windowManager.h"

#include "explosion.h"
#include "bullet.h"
#include "player.h"

#include "EnemyGroup.h"
#include "BasicEnemy.h"
#include "BurstShotEnemy.h"

#include "bre.h"

#include "Updatable.h"
#include "Renderable.h"
#include "MainMenu.h"

int main()
{
    WindowManager window(320, 240, false);
    new StarBackground();
    new MainMenu();
    window.mainLoop();
    return EXIT_SUCCESS;
}
