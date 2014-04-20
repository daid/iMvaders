#include <string.h>
#include <time.h>

#include "engine.h"
#include "windowManager.h"
#include "StarBackground.h"
#include "mainMenu.h"
#include "player.h"
#include "scoreManager.h"
#include "postProcessManager.h"

#include "textDraw.h"
#include "scriptInterface.h"

class WallClock : public Renderable
{
public:
    WallClock() {}
    virtual ~WallClock() {}
    
    virtual void preRender(sf::RenderTarget& window) {}
    virtual void render(sf::RenderTarget& window) {}
    virtual void postRender(sf::RenderTarget& window)
    {
        time_t t = time(NULL);
        struct tm* tt = localtime(&t);
        char buffer[10];
        sprintf(buffer, "%2d:%02d", tt->tm_hour, tt->tm_min);
        drawText(window, 20, 5, buffer, align_left, 0.5);
    }
};

class AutoShutdown : public Updatable, public Renderable
{
    float idleTime;
    const static float idleTimeout = 60 * 60;
    const static float idleTimeoutWarning = 10 * 60;
public:
    AutoShutdown()
    {
        idleTime = 0.0;
    }
    virtual ~AutoShutdown() {}
    
    virtual void update(float delta)
    {
        idleTime += delta;
        
        P<PlayerController> pc[MAX_PLAYERS];
        pc[0] = engine->getObject("playerController1");
        pc[1] = engine->getObject("playerController2");
        for(int n=0; n<MAX_PLAYERS; n++)
        {
            if (pc[n]->up() || pc[n]->down() || pc[n]->left() || pc[n]->right())
                idleTime = 0.0;
            for(int b=0; b<PlayerController::buttonCount; b++)
                if (pc[n]->button(b))
                    idleTime = 0.0;
        }
        
        time_t t = time(NULL);
        struct tm* tt = localtime(&t);
        if (tt->tm_hour < 18)
            idleTime = 0.0;
        
        if (idleTime > idleTimeout)
            system("sudo poweroff");
    }

    virtual void preRender(sf::RenderTarget& window) {}
    virtual void render(sf::RenderTarget& window) {}
    virtual void postRender(sf::RenderTarget& window)
    {
        if (idleTime > idleTimeout - idleTimeoutWarning)
        {
            float f = idleTimeout - idleTime;
            int tsec = int(f) % 60;
            int tmin = int(f) / 60;
            drawText(window, 160, 10, "Shutting down in");
            if (tsec > 9)
                drawText(window, 160, 25, string(tmin) + ":" + string(tsec));
            else
                drawText(window, 160, 25, string(tmin) + ":0" + string(tsec));
        }
    }
};

int main(int argc, char** argv)
{
    new Engine();
    
    P<PlayerController> pc1 = new PlayerController(0);
    P<PlayerController> pc2 = new PlayerController(1);
    engine->registerObject("playerController1", pc1);
    engine->registerObject("playerController2", pc2);
    engine->registerObject("score", new ScoreManager());
    
    bool fullscreen = true;
    for(int n=1; n<argc; n++)
    {
        if (strcmp(argv[n], "-f") == 0)
            fullscreen = true;
        else if (strcmp(argv[n], "-w") == 0)
            fullscreen = false;
    }
    engine->registerObject("windowManager", new WindowManager(320, 240, fullscreen));
    
    new StarBackground();
    new MainMenu();
    new WallClock();
    new AutoShutdown();
    postProcessorManager.triggerPostProcess("crt", -100);
    engine->runMainLoop();
    
    delete engine;
    return 0;
}
 
