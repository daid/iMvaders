#include "explosion.h"
#include "soundManager.h"
#include "random.h"
#include "vectorUtils.h"
#include "main.h"

Explosion::Explosion(glm::vec2 position, float radius, glm::vec2 addedVelocity)
: GameEntity(effectLayer)
{
    soundManager->playSound("explosion.wav", random(0.8, 1.2), radius * 10.0f);
    unsigned int count = int(radius*radius*8);
    particles.reserve(count);
    for(unsigned int n=0; n<count; n++)
    {
        float a = random(0, 360);
        glm::vec2 p = position + vec2FromAngle(random(0, 360)) * random(0, radius);
        glm::vec2 v = vec2FromAngle(a) * random(30, 80) * radius / 10.0f + addedVelocity;
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
    life -= 1.0f / lifeTime * delta;
    if (life < 0.0f)
        destroy();
}

void Explosion::render(sp::RenderTarget& window)
{
    glm::u8vec4 color{255, 255 * (1.0f - life), 0, 255 * life};
    for(unsigned int n=0; n<particles.size(); n++)
    {
        RectangleShape r({1, 1});
        r.setPosition(particles[n].position);
        r.setFillColor(color);
        r.draw(window);
    }
}

