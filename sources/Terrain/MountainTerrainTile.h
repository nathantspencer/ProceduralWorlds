#pragma once
#include <string>
#include <map>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

class MountainTerrainTile
{
public:
    
    void Load(std::string path);
    
    void Draw();
    
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
    };
    
private:
    
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    
    unsigned int m_indexSize;
};
