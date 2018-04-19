#pragma once
#include <string>
#include <map>
#include <glm/glm.hpp>

class Object
{
public:
    void Load(std::string path);
    
    void Bind();
    void Draw();
    void UnBind();
    
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };
    
private:
    unsigned int m_vertexBufferObject;
    unsigned int m_indexBufferObject;
    unsigned int m_indexSize;
};
