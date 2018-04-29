#include <GL/gl3w.h>
#include "MountainTerrainTile.h"
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

inline void* ToVoidPointer(int offset)
{
    size_t offset_ = static_cast<size_t>(offset);
    return reinterpret_cast<void*>(offset_);
}

void MountainTerrainTile::Load(std::string path)
{
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::ifstream file(path);
    std::string str;
    
    Vertex v;
    v.pos = glm::vec3();
    
    while (std::getline(file, str))
    {
        if (strncmp(str.c_str(), "v ", 2) == 0)
        {
            sscanf(str.c_str(), "v %f %f %f", &v.pos.x, &v.pos.y, &v.pos.z);
            v.normal = glm::vec3();
            vertices.push_back(v);
        }
        else if (strncmp(str.c_str(), "f ", 2) == 0)
        {
            unsigned int a, b, c;
            sscanf(str.c_str(), "f %d %d %d", &a, &b, &c);
            indices.push_back(a - 1);
            indices.push_back(b - 1);
            indices.push_back(c - 1);
        }
    }
    
    m_indexSize = indices.size();
    for (int f = 0; f < m_indexSize / 3; ++f)
    {
        unsigned int i = indices[3 * f + 0];
        unsigned int j = indices[3 * f + 1];
        unsigned int k = indices[3 * f + 2];
        glm::vec3 p1 = vertices[i].pos;
        glm::vec3 p2 = vertices[j].pos;
        glm::vec3 p3 = vertices[k].pos;
        glm::vec3 a = p1 - p3;
        glm::vec3 b = p2 - p3;
        glm::vec3 c = glm::cross(a, b);
        vertices[i].normal += c;
        vertices[j].normal += c;
        vertices[k].normal += c;
    }
    
    for (int i = 0; i < vertices.size(); ++i)
    {
        vertices[i].normal = glm::normalize(vertices[i].normal);
    }
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ToVoidPointer(offsetof(Vertex, pos)));
    
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
