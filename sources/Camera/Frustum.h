#pragma once

#include <glm/mat4x4.hpp>

#include <vector>

/*|  Class for computing frustum planes and points.
|*/
class Frustum
{
public:
	/*|  Constructs a frustum.
	|*|
	|*|  @param  nearDistance  distance to the near plane
	|*|  @param  farDistance  distance to the far plane
	|*|  @param  fov  field of view in degrees
	|*/
	Frustum(float nearDistance, float farDistance, float fov);

	/*|  Sets the transform for the frustum.
	|*|
	|*|  @param  transform  frustum's transformation matrix
	|*/
	void SetTransform(const glm::mat4 transform);

	/*|  Retrieves the transformed frustum planes.
	|*|
	|*|  Ordered near, far, left, right, top, bottom.
	|*|
	|*|  @return  a vector of the six frustum planes
	|*/
	std::vector<glm::vec4> GetPlanes();

	/*|  Retrieves a the transformed frustum points.
	|*|
	|*|  TODO: determine and document order
	|*|
	|*|  @return  a vector of the eight frusum points
	|*/
	std::vector<glm::vec3> GetPoints();

private:
	glm::mat4 m_transform;
	std::vector<glm::vec4> planes;
	std::vector<glm::vec3> points;
};