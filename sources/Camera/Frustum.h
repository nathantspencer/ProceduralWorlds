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
		NEAR_PLANE   = 0,
		FAR_PLANE    = 1,
		LEFT_PLANE   = 2,
		RIGHT_PLANE  = 3,
		TOP_PLANE    = 4,
		BOTTOM_PLANE = 5,
		PLANE_COUNT  = 6
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
	void SetTransform(const glm::mat4& transform);

	/*|  Retrieves the transformed frustum planes.
	|*|
	|*|  @return  a vector of the six frustum planes
	|*/
	std::vector<glm::vec4> GetPlanes() const;

	/*|  Retrieves the transformed frustum points.
	|*|
	|*|  @return  a vector of the eight frustum points
	|*/
	std::vector<glm::vec3> GetPoints() const;

	/*|  Tests whether some portion of a bounding box is within the frustum.
	|*|
	|*|  @param  min  the minimum coordinate of the tested bounding box
	|*|  @param  max  the maximum coordinate of the tested bounding box
	|*|
	|*|  @return  true if a portion of the bounding box is within frustum, false otherwise
	|*/
	bool IsBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const;

private:
	glm::mat4 m_transform;

	std::vector<glm::vec4> m_planes;
	std::vector<glm::vec4> m_transformedPlanes;
	
	std::vector<glm::vec3> m_points;
	std::vector<glm::vec3> m_transformedPoints;
};