#include <SFML/OpenGL.hpp>

#include "planet.h"
#include "textureManager.h"
#include "vectorUtils.h"
#include "random.h"
#include "mesh.h"
#include "SpaceRenderer.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_SUBCLASS(Planet, SpaceObject)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setRadius);
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setName);
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setType);
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setDensity);
}
REGISTER_SCRIPT_SUBCLASS(Sun, Planet)
{
}

PVector<Planet> planetList;
PVector<Sun> sunList;

Planet::Planet()
: density(2000000000), radius(128), name("Unknown")
{
    planetList.push_back(this);
    alwaysVisible = true;
    
    textureManager.setTexture(sprite, "Planet1");
    physics = Fixed;

    mass = (radius * radius * radius) * density;
    sprite.setScale(radius/sprite.getTextureRect().width*2, radius/sprite.getTextureRect().height*2);
    angularVelocity = 1.0;
    setRotation(random(0, 360));
}

void Planet::setName(std::string name)
{
    this->name = name;
}
void Planet::setType(std::string type)
{
    textureManager.setTexture(sprite, type);
    sprite.setScale(radius/sprite.getTextureRect().width*2, radius/sprite.getTextureRect().height*2);
}
void Planet::setRadius(float radius)
{
    this->radius = radius;
    mass = (radius * radius * radius) * density;
    sprite.setScale(radius/sprite.getTextureRect().width*2, radius/sprite.getTextureRect().height*2);
}
void Planet::setDensity(float density)
{
    this->density = density;
    mass = (radius * radius * radius) * density;
}

void Planet::update(float delta)
{
    SpaceObject::update(delta);
    
    foreach(SpaceObject, o, spaceObjectList)
    {
        if (o->physics == Newtonian)
        {
            float r = sf::length(o->getPosition() - getPosition());
            if (r < getRadius())
            {
                o->setPosition(getPosition() + (o->getPosition() - getPosition()) / r * (getRadius()+1));
                o->velocity *= powf(0.2, delta);
            }
            o->velocity += gravity(o->getPosition()) * delta;
        }
    }
}

sf::Vector2f Planet::gravity(sf::Vector2f position) const
{
    float r = sf::length(getPosition() - position);
    if (r < getRadius()) return sf::Vector2f(0, 0);
    return ((getPosition() - position) / r) * (mass * G) / powf(r, 2.0);
}

sf::Vector2f Planet::gravity(sf::Vector2f position, float deltaTime)
{
    sf::Vector2f p = predictPositionAtDelta(deltaTime);
    float r = sf::length(p - position);
    if (r < getRadius()) return sf::Vector2f(0, 0);
    return ((p - position) / r) * (mass * G) / powf(r, 2.0);
}

float Planet::calcOrbitVelocity(float distance) const
{
    return sqrtf((mass*G)/distance);
}
float Planet::calcOrbitTime(float distance) const
{
    return 2*M_PI*sqrtf(distance*distance*distance/(mass*G));
}
float Planet::sphereOfInfluence() const
{
    if (physics != Orbit)
        return INFINITY;
    return orbitDistance * powf(mass / orbitTarget->mass, 2.0f/5.0f);
}
float Planet::hillSphereRadius() const
{
    if (physics != Orbit)
        return INFINITY;
    return orbitDistance * powf(mass / (3 * orbitTarget->mass), 1.0f/3.0f);
}

class SphereMesh : public Mesh
{
    int subDivisions;
public:
    SphereMesh(int subDivisions)
    : subDivisions(subDivisions)
    {
        vertexList.push_back(TVertex(sf::Vector3f( 1, 0, 0), sf::Vector2f(0.00, 0.5)));//0
        vertexList.push_back(TVertex(sf::Vector3f( 0, 1, 0), sf::Vector2f(0.25, 0.5)));//1
        vertexList.push_back(TVertex(sf::Vector3f( 0, 0, 1), sf::Vector2f(0.00,   0)));//2
        vertexList.push_back(TVertex(sf::Vector3f( 0, 0,-1), sf::Vector2f(0.00, 1.0)));//3

        vertexList.push_back(TVertex(sf::Vector3f(-1, 0, 0), sf::Vector2f(0.50, 0.5)));//4
        vertexList.push_back(TVertex(sf::Vector3f( 0, 0, 1), sf::Vector2f(0.25,   0)));//5
        vertexList.push_back(TVertex(sf::Vector3f( 0, 0,-1), sf::Vector2f(0.25, 1.0)));//6

        vertexList.push_back(TVertex(sf::Vector3f( 0,-1, 0), sf::Vector2f(0.75, 0.5)));//7
        vertexList.push_back(TVertex(sf::Vector3f( 0, 0, 1), sf::Vector2f(0.50,   0)));//8
        vertexList.push_back(TVertex(sf::Vector3f( 0, 0,-1), sf::Vector2f(0.50, 1.0)));//9

        vertexList.push_back(TVertex(sf::Vector3f( 1, 0, 0), sf::Vector2f(1.00, 0.5)));//10
        vertexList.push_back(TVertex(sf::Vector3f( 0, 0, 1), sf::Vector2f(0.75,   0)));//11
        vertexList.push_back(TVertex(sf::Vector3f( 0, 0,-1), sf::Vector2f(0.75, 1.0)));//12
        
        addSphereSubdivide(1, 0, 2);
        addSphereSubdivide(0, 1, 3);
        addSphereSubdivide(4, 1, 5);
        addSphereSubdivide(1, 4, 6);
        
        addSphereSubdivide(7, 4, 8);
        addSphereSubdivide(4, 7, 9);
        addSphereSubdivide(10, 7, 11);
        addSphereSubdivide(7, 10, 12);
        
        for(unsigned int n=0; n<vertexList.size(); n++)
        {
            vertexList[n].normal = sf::normalize(vertexList[n].position);
            //Recalculate the U values of the uvMap so we get proper spherical mapping.
            float u = vertexList[n].uvMap.x;
            float v = fabs(vertexList[n].uvMap.y - 0.5) * 2.0; // = 0 when at center, 1 at top/bottom of the texture.
            if (v < 1.0)
                u = fmod(u, 0.25) / (1.0 - v) + (floorf(u * 4) / 4.0f);
            
            vertexList[n].uvMap.x = u;
        }
    }
    
private:
    void addSphereSubdivide(int idx0, int idx1, int idx2, int depth = 0)
    {
        if (depth == subDivisions)
        {
            faceList.push_back(TFace(idx0, idx1, idx2));
        }else{
            sf::Vector3f v01 = sf::normalize(vertexList[idx0].position + vertexList[idx1].position);
            sf::Vector3f v12 = sf::normalize(vertexList[idx1].position + vertexList[idx2].position);
            sf::Vector3f v20 = sf::normalize(vertexList[idx2].position + vertexList[idx0].position);
            sf::Vector2f uv01 = (vertexList[idx0].uvMap + vertexList[idx1].uvMap) / 2.0f;
            sf::Vector2f uv12 = (vertexList[idx1].uvMap + vertexList[idx2].uvMap) / 2.0f;
            sf::Vector2f uv20 = (vertexList[idx2].uvMap + vertexList[idx0].uvMap) / 2.0f;
            
            int idx01 = vertexList.size();
            vertexList.push_back(TVertex(v01, uv01));
            int idx12 = vertexList.size();
            vertexList.push_back(TVertex(v12, uv12));
            int idx20 = vertexList.size();
            vertexList.push_back(TVertex(v20, uv20));
            
            addSphereSubdivide(idx0, idx01, idx20, depth+1);
            addSphereSubdivide(idx1, idx12, idx01, depth+1);
            addSphereSubdivide(idx2, idx20, idx12, depth+1);
            addSphereSubdivide(idx01, idx12, idx20, depth+1);
        }
    }
};

static SphereMesh* sphereMeshList[10];
void Planet::render3D(RenderInfo* info)
{
    glColor4f(0.3, 0.5, 0.7, 1);
    textureManager.getTexture("PlanetTexture1")->setSmooth(true);
    sf::Texture::bind(textureManager.getTexture("PlanetTexture1"), sf::Texture::Pixels);
    sf::Shader::bind(lightShader);
    
    glScalef(radius, radius, radius);
    
    float f = info->objectDepth / radius;
    if (f < -1.0)
        return;

    //Scale the subdivision amount depending on the visible scale of the planet.
    int idx = 0;
    if (f < 8)
        idx = 5;
    else if (f < 10)
        idx = 4;
    else if (f < 25)
        idx = 3;
    else if (f < 75)
        idx = 2;
    else if (f < 150)
        idx = 1;
    
    if (sphereMeshList[idx] == NULL)
        sphereMeshList[idx] = new SphereMesh(idx);
    glRotatef(getRotation(), 0, 0, 1);
    sphereMeshList[idx]->draw();
}

Sun::Sun()
: Planet()
{
    setName("Sun");
    //Make the sun sprite bigger, as the outer radius of the sun is softer then of planets.
    textureManager.setTexture(sprite, "Sun");
    
    sunList.push_back(this);
}

void Sun::renderOnRadar(sf::RenderTarget& window)
{
    sprite.setScale(getRadius()/sprite.getTextureRect().width*2, getRadius()/sprite.getTextureRect().height*2);
    sprite.setScale(sprite.getScale() * 1.5f);

    Planet::renderOnRadar(window);
}

void Sun::render3D(RenderInfo* info)
{
    sf::Texture::bind(textureManager.getTexture("Sun"), sf::Texture::Pixels);
    sf::Shader::bind(NULL);
    
    glDepthFunc(GL_ALWAYS);
    glScalef(getRadius(), getRadius(), getRadius());
    glColor4f(1,1,1,1);
    glRotatef(info->yaw, 0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(  0,   0); glVertex3f(-1.5, 0,-1.5);
    glTexCoord2f(512,   0); glVertex3f( 1.5, 0,-1.5);
    glTexCoord2f(512, 512); glVertex3f( 1.5, 0, 1.5);
    glTexCoord2f(  0, 512); glVertex3f(-1.5, 0, 1.5);
    glEnd();
    glDepthFunc(GL_LESS);
}

bool checkLineOfSight(sf::Vector2f start, sf::Vector2f end)
{
    if ((start - end) < 0.001f)
    {
        foreach(Planet, p, planetList)
            if (p->getPosition() - start < p->getRadius() * 0.9f)
                return false;
        return true;
    }
    foreach(Planet, p, planetList)
    {
        float d = sf::length(start - end);
        float f = sf::dot(end - start, p->getPosition() - start) / d;
        if (f < 0) f = 0;
        if (f > d) f = d;
        sf::Vector2f q = start + sf::normalize(end - start) * f;
        if ((q - p->getPosition()) < p->getRadius() * 0.9f)
            return false;
    }
    return true;
}

bool findPathAroundPlanets(sf::Vector2f start, sf::Vector2f& end)
{
    if ((start - end) < 0.001f)
        return false;

    Planet* firstAvoid = NULL;
    float startEndLength = sf::length(start - end);
    float firstAvoidF = startEndLength;
    sf::Vector2f firstAvoidQ;
    foreach(Planet, p, planetList)
    {
        float f = sf::dot(end - start, p->getPosition() - start) / startEndLength;
        if (f < 0) continue;
        if (f > startEndLength) continue;
        sf::Vector2f q = start + (end - start) / startEndLength * f;
        if ((q - p->getPosition()) < p->getRadius() * 2.0f)
        {
            if (firstAvoid == NULL || f < firstAvoidF)
            {
                firstAvoid = *p;
                firstAvoidF = f;
                firstAvoidQ = q;
            }
        }
    }
    if (firstAvoid)
    {
        end = firstAvoid->getPosition() + sf::normalize(firstAvoidQ - firstAvoid->getPosition()) * firstAvoid->getRadius() * 2.0f;
        return true;
    }
    return false;
}

void validateOrbits()
{
    foreach(Planet, p, planetList)
    {
        if (p->physics != SpaceObject::Orbit)
            continue;
        if (p->getRadius() > p->hillSphereRadius() / 3.0)
            printf("%s size is bigger then the minimum orbit distance\n", p->getName().c_str());
        float r = p->orbitTarget->hillSphereRadius();
        if (p->orbitDistance < r/3.0 && r != INFINITY)
            printf("%s too close to %s (%f < %f)\n", p->getName().c_str(), p->orbitTarget->getName().c_str(), p->orbitDistance, r/3.0);
        if (p->orbitDistance > r/2.0)
            printf("%s too far from %s (%f > %f)\n", p->getName().c_str(), p->orbitTarget->getName().c_str(), p->orbitDistance, r/2.0);
        
        foreach(Planet, p2, planetList)
        {
            if (p == p2 || p2->physics != SpaceObject::Orbit || p2->orbitTarget != p->orbitTarget)
                continue;
            r = p2->hillSphereRadius() / 2.0 + p->hillSphereRadius() / 2.0;
            if (fabs(p2->orbitDistance - p->orbitDistance) < r && !(p2->orbitDistance == p->orbitDistance && p->orbitAngle != p2->orbitAngle))
                printf("%s SoI collides with SoI of %s (%f < %f)\n", p->getName().c_str(), p2->getName().c_str(), fabs(p2->orbitDistance - p->orbitDistance), r);
        }
    }
    foreach(SpaceObject, obj, spaceObjectList)
    {
        if (obj->physics != SpaceObject::Orbit)
            continue;
        P<Planet> p = obj;
        if (p)
            continue;
        
        float r = obj->orbitTarget->hillSphereRadius();
        if (obj->orbitDistance < r/3.0 && r != INFINITY)
            printf("%p too close to %s (%f < %f)\n", *obj, obj->orbitTarget->getName().c_str(), obj->orbitDistance, r/3.0);
        if (obj->orbitDistance > r/2.0)
            printf("%p too far from %s (%f > %f)\n", *obj, obj->orbitTarget->getName().c_str(), obj->orbitDistance, r/2.0);
    }
}
