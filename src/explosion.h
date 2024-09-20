#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "gameEntity.h"

class Particle
{
public:
    glm::vec2 position{};
    glm::vec2 velocity{};
    
    Particle(glm::vec2 position, glm::vec2 velocity)
    : position(position), velocity(velocity)
    {}
};

class Explosion : public GameEntity
{
private:
    std::vector<Particle> particles;
    float life;
    static constexpr float lifeTime = 0.5;
public:
    Explosion(glm::vec2 position, float radius, glm::vec2 addedVelocity = glm::vec2(0, 0));
    virtual ~Explosion();
    
    virtual void update(float delta) override;
    
    virtual void render(sp::RenderTarget& window) override;
};

#endif//EXPLOSION_H
