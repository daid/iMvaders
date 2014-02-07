#include <SFML/OpenGL.hpp>

#include "Mesh.h"

void Mesh::draw()
{
    glBegin(GL_TRIANGLES);
    for(unsigned int i=0; i<faceList.size(); i++)
    {
        TFace f = faceList[i];
        glTexCoord2f(vertexList[f.index[0]].uvMap.x, vertexList[f.index[0]].uvMap.y);
        glNormal3f(vertexList[f.index[0]].normal.x, vertexList[f.index[0]].normal.y, vertexList[f.index[0]].normal.z);
        glVertex3f(vertexList[f.index[0]].position.x, vertexList[f.index[0]].position.y, vertexList[f.index[0]].position.z);

        glTexCoord2f(vertexList[f.index[1]].uvMap.x, vertexList[f.index[1]].uvMap.y);
        glNormal3f(vertexList[f.index[1]].normal.x, vertexList[f.index[1]].normal.y, vertexList[f.index[1]].normal.z);
        glVertex3f(vertexList[f.index[1]].position.x, vertexList[f.index[1]].position.y, vertexList[f.index[1]].position.z);

        glTexCoord2f(vertexList[f.index[2]].uvMap.x, vertexList[f.index[2]].uvMap.y);
        glNormal3f(vertexList[f.index[2]].normal.x, vertexList[f.index[2]].normal.y, vertexList[f.index[2]].normal.z);
        glVertex3f(vertexList[f.index[2]].position.x, vertexList[f.index[2]].position.y, vertexList[f.index[2]].position.z);
    }
    glEnd();
}
