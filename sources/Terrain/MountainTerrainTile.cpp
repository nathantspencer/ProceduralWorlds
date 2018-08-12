#include "MountainTerrainTile.h"

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>

inline void* ToVoidPointer(int offset)
{
    size_t offset_ = static_cast<size_t>(offset);
    return reinterpret_cast<void*>(offset_);
}

MountainTerrainTile::MountainTerrainTile() : MountainTerrainTile(9)
{
}

MountainTerrainTile::MountainTerrainTile(size_t dimension)
{
    m_dimension = dimension;
    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;
    Vertex v;
    
    for (int x = 0; x < m_dimension; ++x)
    {
        for (int y = 0; y < m_dimension; ++y)
        {
            float vx = -1.0f + 2.0f * x / (m_dimension - 1);
            float vz = -1.0f + 2.0f * y / (m_dimension - 1);
            v.position = glm::ivec3(vx, 0.0f, vz);
            vertices.push_back(v);
        }
    }
    
    for (int x = 0; x < m_dimension - 1; ++x)
    {
        for (int y = 0; y < m_dimension - 1; ++y)
        {
            unsigned int i1 = y * m_dimension + x;
            unsigned int i2 = i1 + 1;
            unsigned int i3 = i1 + m_dimension;
            unsigned int i4 = i2 + m_dimension;
            
            glm::vec3 p1 = vertices[i1].position;
            glm::vec3 p2 = vertices[i2].position;
            glm::vec3 p3 = vertices[i3].position;
            glm::vec3 p4 = vertices[i4].position;

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i4);
        
            glm::vec3 a = p1 - p4;
            glm::vec3 b = p2 - p4;
            glm::vec3 c = glm::cross(a, b);
            vertices[i1].normal += c;
            vertices[i2].normal += c;
            vertices[i4].normal += c;
            
            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i4);
            
            b = p3 - p4;
            c = glm::cross(a, b);
            vertices[i1].normal += c;
            vertices[i3].normal += c;
            vertices[i4].normal += c;
        }
    }
    
    for (int i = 0; i < vertices.size(); ++i)
    {
        vertices[i].normal = glm::normalize(vertices[i].normal);
    }
    
    m_indexSize = indices.size();
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ToVoidPointer(offsetof(Vertex, position)));
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ToVoidPointer(offsetof(Vertex, normal)));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void MountainTerrainTile::Draw()
{
    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, m_indexSize, GL_UNSIGNED_INT, 0, 10000);
    glBindVertexArray(0);
}
