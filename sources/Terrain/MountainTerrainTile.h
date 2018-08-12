#pragma once

#include <string>
#include <map>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

class MountainTerrainTile
{
public:
    
    MountainTerrainTile();
    MountainTerrainTile(size_t dimension);
    
    void Draw();
    
private:
    
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
    };
    
    size_t m_dimension;
    size_t m_indexSize;
    
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
};
