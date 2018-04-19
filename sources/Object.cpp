#include <GL/gl3w.h>
#include "Object.h"
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

void Object::Load(std::string path)
{
    glGenBuffers(1, &m_vertexBufferObject);
    glGenBuffers(1, &m_indexBufferObject);
    
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    std::ifstream file(path);
    std::string str;
    Vertex v;
    v.pos = glm::vec3();
    while (std::getline(file, str))
    {
        if (strncmp(str.c_str(), "v ", 2) == 0)
        {
            sscanf(str.c_str(), "v %f %f %f", &v.pos.x, &v.pos.y, &v.pos.z);
            v.normal = glm::vec3(0);
            v.uv = glm::vec2();
            vertices.push_back(v);
        }
        else if (strncmp(str.c_str(), "f ", 2) == 0)
        {
            int a, b, c;
            sscanf(str.c_str(), "f %d %d %d", &a, &b, &c);
            indices.push_back(a - 1);
            indices.push_back(b - 1);
            indices.push_back(c - 1);
        }
    }
    m_indexSize = indices.size();
    
    for (int f = 0; f < m_indexSize / 3; ++f)
    {
        int i = indices[3 * f + 0];
        int j = indices[3 * f + 1];
        int k = indices[3 * f + 2];
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
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Object::Draw()
{
    glDrawElements(GL_TRIANGLES, m_indexSize, GL_UNSIGNED_INT, 0);
}

void Object::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);
}

void Object::UnBind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
