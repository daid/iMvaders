#ifndef MESH_H
#define MESH_H

#include <vector>
#include <SFML/System.hpp>

class Mesh
{
protected:
    struct TVertex
    {
        sf::Vector3f position;
        sf::Vector2f uvMap;
        sf::Vector3f normal;
        
        TVertex(sf::Vector3f position, sf::Vector2f uvMap)
        : position(position), uvMap(uvMap) {}
    };
    struct TFace
    {
        int index[3];
        TFace(int idx0, int idx1, int idx2)
        { index[0] = idx0; index[1] = idx1; index[2] = idx2; }
    };
    
    std::vector<TVertex> vertexList;
    std::vector<TFace> faceList;
public:
    void draw();
};

#endif//MESH_H
