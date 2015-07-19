#include <string.h>
#include <time.h>

#include "engine.h"
#include "main.h"
#include "StarBackground.h"
#include "mainMenu.h"
#include "player.h"
#include "scoreManager.h"

#include "textDraw.h"
#include "scriptInterface.h"

RenderLayer* backgroundLayer;
RenderLayer* objectLayer;
RenderLayer* effectLayer;
RenderLayer* hudLayer;
PostProcessor* glitchPostProcessor;
PostProcessor* crtPostProcessor;

class WallClock : public Renderable
{
public:
    WallClock() : Renderable(hudLayer) {}
    virtual ~WallClock() {}
    
    virtual void render(sf::RenderTarget& window)
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
    constexpr static float idleTimeout = 60 * 60;
    constexpr static float idleTimeoutWarning = 10 * 60;
public:
    AutoShutdown()
    : Renderable(hudLayer)
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

    virtual void render(sf::RenderTarget& window)
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

class GlitchHandler : public Updatable, public EventHandler
{
    float magtitude;
public:
    GlitchHandler() : EventHandler("glitch") { magtitude = 0; }
    virtual ~GlitchHandler() {}
    
    virtual void event(string eventName, void* param)
    {
        magtitude = 10.0;
    }
    
    virtual void update(float delta)
    {
        if (magtitude > 0)
            magtitude -= delta * 10.0;
        else
            magtitude = 0;
        glitchPostProcessor->enabled = magtitude > 0;
        glitchPostProcessor->setUniform("magtitude", magtitude);
        glitchPostProcessor->setUniform("delta", random(0, 100));
    }
};

int main(int argc, char** argv)
{
    new Engine();
    
    new DirectoryResourceProvider("resources/");
    
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

    //Setup the rendering layers.
    backgroundLayer = new RenderLayer();
    objectLayer = new RenderLayer(backgroundLayer);
    effectLayer = new RenderLayer(objectLayer);
    hudLayer = new RenderLayer(effectLayer);
    glitchPostProcessor = new PostProcessor("glitch", hudLayer);
    crtPostProcessor = new PostProcessor("crt", glitchPostProcessor);
    defaultRenderLayer = objectLayer;

#ifdef DEBUG
    new CollisionDebugDraw(hudLayer);
#endif

    engine->registerObject("glitchHandler", new GlitchHandler());
    engine->registerObject("windowManager", new WindowManager(320, 240, fullscreen, crtPostProcessor));
        
    new StarBackground();
    new MainMenu();
    new WallClock();
    new AutoShutdown();
    
    engine->runMainLoop();
    
    delete engine;
    return 0;
}
 
