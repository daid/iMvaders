#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <algorithm>

#include "SpaceRenderer.h"
#include "textureManager.h"
#include "vectorUtils.h"
#include "random.h"
#include "planet.h"
#include "engine.h"
#include "Player.h"

std::vector<NebulaInfo> nebulaInfo;
std::vector<sf::Vector3f> spaceDust;
sf::Shader* lightShader;

struct RenderSortInfo
{
    float objectDepth;
    SpaceObject* obj;
};

bool renderCompare(RenderSortInfo i0, RenderSortInfo i1)
{
    return i0.objectDepth > i1.objectDepth;
}

void renderSpace(sf::Vector3f cameraPosition, float yaw, float pitch, float roll)
{
    glClearDepth(1.f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.f, 320.0/240.0, 1.f, 16000.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(90, 1, 0, 0);
    glScalef(1,1,-1);
    glRotatef(-roll, 0,1, 0);
    glRotatef(-pitch, 0, 1, 0);
    glRotatef(-yaw, 0, 0, 1);
    
    textureManager.getTexture("Stars")->setSmooth(true);
    sf::Texture::bind(textureManager.getTexture("Stars"), sf::Texture::Pixels);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f( 100, 100, 100);
    glTexCoord2f(   0,    0); glVertex3f( 100, 100,-100);
    glTexCoord2f(1024, 1024); glVertex3f(-100, 100, 100);
    glTexCoord2f(   0, 1024); glVertex3f(-100, 100,-100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f(-100, 100, 100);
    glTexCoord2f(   0,    0); glVertex3f(-100, 100,-100);
    glTexCoord2f(1024, 1024); glVertex3f(-100,-100, 100);
    glTexCoord2f(   0, 1024); glVertex3f(-100,-100,-100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f(-100,-100, 100);
    glTexCoord2f(   0,    0); glVertex3f(-100,-100,-100);
    glTexCoord2f(1024, 1024); glVertex3f( 100,-100, 100);
    glTexCoord2f(   0, 1024); glVertex3f( 100,-100,-100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f( 100,-100, 100);
    glTexCoord2f(   0,    0); glVertex3f( 100,-100,-100);
    glTexCoord2f(1024, 1024); glVertex3f( 100, 100, 100);
    glTexCoord2f(   0, 1024); glVertex3f( 100, 100,-100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f( 100,-100, 100);
    glTexCoord2f(   0,    0); glVertex3f(-100,-100, 100);
    glTexCoord2f(1024, 1024); glVertex3f( 100, 100, 100);
    glTexCoord2f(   0, 1024); glVertex3f(-100, 100, 100);
    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(1024,    0); glVertex3f( 100,-100,-100);
    glTexCoord2f(   0,    0); glVertex3f(-100,-100,-100);
    glTexCoord2f(1024, 1024); glVertex3f( 100, 100,-100);
    glTexCoord2f(   0, 1024); glVertex3f(-100, 100,-100);
    glEnd();
    
    for(unsigned int n=0; n<nebulaInfo.size(); n++)
    {
        sf::Texture::bind(textureManager.getTexture(nebulaInfo[n].textureName), sf::Texture::Pixels);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glPushMatrix();
        glRotatef(180, nebulaInfo[n].vector.x, nebulaInfo[n].vector.y, nebulaInfo[n].vector.z);
        glColor4f(1,1,1,0.3);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(1024,    0); glVertex3f( 100, 100, 100);
        glTexCoord2f(   0,    0); glVertex3f( 100, 100,-100);
        glTexCoord2f(1024, 1024); glVertex3f(-100, 100, 100);
        glTexCoord2f(   0, 1024); glVertex3f(-100, 100,-100);
        glEnd();
        glPopMatrix();
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    sf::Texture::bind(NULL);
    
    sf::Vector2f cameraPosition2D(cameraPosition.x, cameraPosition.y);
    glDisable(GL_DEPTH_TEST);

    //Shade the objects with light from the first sun.
    //TODO: Fix for multiple suns.
    P<Sun> sun = sunList[0];
    
    if (lightShader == NULL)
    {
        lightShader = new sf::Shader();
        lightShader->loadFromFile("resources/perPixelLight.vert", "resources/perPixelLight.frag");
    }
    
    sf::Vector2f viewVector = sf::vector2FromAngle(yaw);
    std::vector<RenderSortInfo> renderList;
    foreach(SpaceObject, obj, spaceObjectList)
    {
        RenderSortInfo info;
        info.obj = *obj;
        info.objectDepth = sf::dot(viewVector, obj->getPosition() - cameraPosition2D);
        
        renderList.push_back(info);
    }
    
    std::sort(renderList.begin(), renderList.end(), renderCompare);
    
    RenderInfo info;
    info.cameraPosition = cameraPosition2D;
    info.yaw = yaw;
    info.pitch = pitch;
    info.roll = roll;
    
    for(unsigned int n=0; n<renderList.size(); n++)
    {
        SpaceObject* obj = renderList[n].obj;
        sf::Vector2f lightDir = sf::normalize(sun->getPosition() - obj->getPosition());
        info.objectDepth = renderList[n].objectDepth;
        
        glPushMatrix();
        GLfloat lightpos[] = {lightDir.x, lightDir.y, 0, 0};
        glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
        info.renderScale = 1;
        
        //On far away objects, increase the renderScale, which will scale down the distance so the objects are not beyond the Z clipping plane.
        // Note that no depth-buffer drawing will be enabled when renderScale > 1 which should not be a problem as only planets should be visible at this distance.
        while(info.objectDepth / info.renderScale > 8000)
            info.renderScale *= 2;
        if (info.renderScale > 1)
        {
            glScalef(1.0 / float(info.renderScale), 1.0 / float(info.renderScale), 1.0 / float(info.renderScale));
            glDepthMask(false);
            glDisable(GL_DEPTH_TEST);
        }else{
            glDepthMask(true);
            glEnable(GL_DEPTH_TEST);
        }
        glTranslatef(-cameraPosition.x,-cameraPosition.y, -cameraPosition.z);
        glTranslatef(obj->getPosition().x, obj->getPosition().y, 0);
        obj->render3D(&info);
        
        glPopMatrix();
    }
    sf::Shader::bind(NULL);
    
    sf::Texture::bind(NULL);
    
    while(spaceDust.size() < 2000)
        spaceDust.push_back(sf::Vector3f());
    
    P<PlayerVessel> player = engine->getObject("player");
    sf::Vector2f dustVector = player->velocity / 30.0f;
    glDepthMask(false);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glTranslatef(-cameraPosition.x,-cameraPosition.y, -cameraPosition.z);
    glColor4f(0.7, 0.5, 0.35, 0.1);
    for(unsigned int n=0; n<spaceDust.size(); n++)
    {
        const float maxDustDist = 250.0f;
        const float minDustDist = 30.0f;
        const float dustScale = 0.2f;
        glPushMatrix();
        if ((spaceDust[n] - cameraPosition) > maxDustDist || (spaceDust[n] - cameraPosition) < minDustDist)
            spaceDust[n] = cameraPosition + sf::Vector3f(random(-maxDustDist, maxDustDist), random(-maxDustDist, maxDustDist), random(-maxDustDist, maxDustDist));
        glTranslatef(spaceDust[n].x, spaceDust[n].y, spaceDust[n].z);
        glBegin(GL_LINES);
        glVertex3f(-dustVector.x, -dustVector.y, 0);
        glVertex3f( dustVector.x,  dustVector.y, 0);
        glEnd();
        glPopMatrix();
    }
    glDepthMask(true);
}
