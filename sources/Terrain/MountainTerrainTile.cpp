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
	m_min = glm::vec3(std::numeric_limits<float>::max());
	m_min = glm::vec3(std::numeric_limits<float>::min());

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
			v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			vertices.push_back(v);

			m_min = glm::min(m_min, v.position);
			m_max = glm::max(m_max, v.position);
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

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i4);
            
            indices.push_back(i1);
            indices.push_back(i4);
            indices.push_back(i3);
        }
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

std::pair<glm::vec3, glm::vec3> MountainTerrainTile::GetBoundingBox() const
{
	return std::pair<glm::vec3, glm::vec3>();
}
