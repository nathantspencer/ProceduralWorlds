#pragma once

#include <glm/mat4x4.hpp>

#include <vector>

/*|  Class for computing frustum planes and points.
|*/
class Frustum
{
public:
	/*|  Enumeration of frustum planes.
	|*|
	|*|  Use as indices to the frustum plane list.
	|*|  
	|*|  @see GetPlanes
	|*/
	enum Planes
	{
		NEAR        = 0,
		FAR         = 1,
		LEFT        = 2,
		RIGHT       = 3,
		TOP         = 4,
		BOTTOM      = 5,
		PLANE_COUNT = 6
	};

	/*|  Enumeration of frustum points.
	|*|
	|*|  Use as indices to the frustum point list.
	|*|
	|*|  @see GetPoints
	|*/
	enum Points
	{
		FAR_TOP_LEFT      = 0,
		FAR_TOP_RIGHT     = 1,
		FAR_BOTTOM_LEFT   = 2,
		FAR_BOTTOM_RIGHT  = 3,
		NEAR_TOP_LEFT     = 4,
		NEAR_TOP_RIGHT    = 5,
		NEAR_BOTTOM_LEFT  = 6,
		NEAR_BOTTOM_RIGHT = 7,
		POINT_COUNT       = 8
	};

	/*|  Constructs a frustum.
	|*|
	|*|  @param  nearDistance  distance to the near plane
	|*|  @param  farDistance  distance to the far plane
	|*|  @param  fov  field of view in degrees
	|*|  @param  aspect  view aspect ratio
	|*/
	Frustum(float nearDistance, float farDistance, float fov, float aspect);

	/*|  Sets the transform for the frustum.
	|*|
	|*|  @param  transform  frustum's transformation matrix
	|*/
	void SetTransform(const glm::mat4 transform);

	/*|  Retrieves the transformed frustum planes.
	|*|
	|*|  Use sparingly where possible; each plane is transformed on demand.
	|*|
	|*|  @return  a vector of the six frustum planes
	|*/
	std::vector<glm::vec4> GetPlanes();

	/*|  Retrieves the transformed frustum points.
	|*|
	|*|  Use sparingly where possible; each point is transformed on demand.
	|*|
	|*|  @return  a vector of the eight frusum points
	|*/
	std::vector<glm::vec3> GetPoints();

private:
	glm::mat4 m_transform;
	std::vector<glm::vec4> m_planes;
	std::vector<glm::vec3> m_points;
};