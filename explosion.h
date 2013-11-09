
class Particle
{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    
    Particle(sf::Vector2f position, sf::Vector2f velocity)
    : position(position), velocity(velocity)
    {}
};

class Explosion : public GameEntity
{
private:
    sf::Sound sound;
    std::vector<Particle> particles;
    float life;
public:
    Explosion(sf::Vector2f position, float radius)
    : sound(explosionSound)
    {
        sound.setVolume(radius * 10.0);
        sound.play();
        unsigned int count = int(radius*radius*8);
        particles.reserve(count);
        for(unsigned int n=0; n<count; n++)
        {
            float a = random(0, 360);
            sf::Vector2f p = position + sf::vector2FromAngle(random(0, 360)) * random(0, radius);
            sf::Vector2f v = sf::vector2FromAngle(a) * random(0.5, 1.2) * radius / 10.0f;
            particles.push_back(Particle(p, v));
        }
        life = 1.0;
    }
    virtual ~Explosion() {}
    
    virtual void update()
    {
        for(unsigned int n=0; n<particles.size(); n++)
        {
            particles[n].position += particles[n].velocity;
        }
        life -= 0.03;
        if (life < 0.0)
            destroy();
    }
    
    virtual void postRender(sf::RenderTarget& window)
    {
        sf::RectangleShape rect(sf::Vector2f(1, 1));
        for(unsigned int n=0; n<particles.size(); n++)
        {
            rect.setPosition(particles[n].position);
            rect.setFillColor(sf::Color(255, 255 * (1.0-life), 0, 255 * life));
            window.draw(rect);
        }
    }
};
