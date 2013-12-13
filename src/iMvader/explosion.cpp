#include "explosion.h"
#include "soundManager.h"
#include "random.h"
#include "vectorUtils.h"

Explosion::Explosion(sf::Vector2f position, float radius)
{
    soundManager.setSound(sound, "explosion");
    sound.setVolume(radius * 10.0);
    sound.setPitch(random(0.8, 1.2));
    sound.play();
    unsigned int count = int(radius*radius*8);
    particles.reserve(count);
    for(unsigned int n=0; n<count; n++)
    {
        float a = random(0, 360);
        sf::Vector2f p = position + sf::vector2FromAngle(random(0, 360)) * random(0, radius);
        sf::Vector2f v = sf::vector2FromAngle(a) * random(30, 80) * radius / 10.0f;
        particles.push_back(Particle(p, v));
    }
    life = 1.0;
}
Explosion::~Explosion() {}
    
void Explosion::update(float delta)
{
    for(unsigned int n=0; n<particles.size(); n++)
    {
        particles[n].position += particles[n].velocity * delta;
    }
    life -= 1.0 / lifeTime * delta;
    if (life < 0.0)
        destroy();
}

void Explosion::postRender(sf::RenderTarget& window)
{
    sf::VertexArray tmpArray(sf::Quads, particles.size() * 4);
    sf::Color color(255, 255 * (1.0 - life), 0, 255 * life);
    for(unsigned int n=0; n<particles.size(); n++)
    {
        tmpArray[n*4+0].position = particles[n].position + sf::Vector2f( 0.5, 0.5);
        tmpArray[n*4+1].position = particles[n].position + sf::Vector2f(-0.5, 0.5);
        tmpArray[n*4+2].position = particles[n].position + sf::Vector2f(-0.5,-0.5);
        tmpArray[n*4+3].position = particles[n].position + sf::Vector2f( 0.5,-0.5);
        tmpArray[n*4+0].color = color;
        tmpArray[n*4+1].color = color;
        tmpArray[n*4+2].color = color;
        tmpArray[n*4+3].color = color;
    }
    window.draw(tmpArray);
}

