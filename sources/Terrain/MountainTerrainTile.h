#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include <map>
#include <string>

class MountainTerrainTile
{
public:
    
    MountainTerrainTile();
    MountainTerrainTile(size_t dimension);
    
    void Draw();

	std::pair<glm::vec3, glm::vec3> GetBoundingBox() const;
    
private:
    
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
    };
    
	glm::vec3 m_min;
	glm::vec3 m_max;

    size_t m_dimension;
    size_t m_indexSize;
    
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
};
