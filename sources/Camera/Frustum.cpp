#include "Frustum.h"

#include <Common/Math/Constants.h>


Frustum::Frustum(float nearDistance, float farDistance, float fov, float aspect)
{
	m_planes.resize(PLANE_COUNT);
	m_points.resize(POINT_COUNT);

	glm::vec3 cameraForward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);

	glm::vec3 nearCenter = cameraForward * nearDistance;
	glm::vec3 farCenter = cameraForward * farDistance;

	float fovRadians = fov * Constants::PI / 180;
	float nearHeight = 2 * tan(fovRadians / 2) * nearDistance;
	float farHeight = 2 * tan(fovRadians / 2) * farDistance;
	float nearWidth = nearHeight * aspect;
	float farWidth = farHeight * aspect;

	// Calculate frustum points
	glm::vec3 farTopLeft = farCenter + cameraUp * (farHeight * 0.5f) - cameraRight * (farWidth * 0.5f);
	glm::vec3 farTopRight = farCenter + cameraUp * (farHeight * 0.5f) + cameraRight * (farWidth * 0.5f);
	glm::vec3 farBottomLeft = farCenter - cameraUp * (farHeight * 0.5f) - cameraRight * (farWidth * 0.5f);
	glm::vec3 farBottomRight = farCenter - cameraUp * (farHeight * 0.5f) + cameraRight * (farWidth * 0.5f);

	glm::vec3 nearTopLeft = nearCenter + cameraUp * (nearHeight * 0.5f) - cameraRight * (nearWidth * 0.5f);
	glm::vec3 nearTopRight = nearCenter + cameraUp * (nearHeight * 0.5f) + cameraRight * (nearWidth * 0.5f);
	glm::vec3 nearBottomLeft = nearCenter - cameraUp * (nearHeight * 0.5f) - cameraRight * (nearWidth * 0.5f);
	glm::vec3 nearBottomRight = nearCenter - cameraUp * (nearHeight * 0.5f) + cameraRight * (nearWidth * 0.5f);

	m_points[FAR_TOP_LEFT] = farTopLeft;
	m_points[FAR_TOP_RIGHT] = farTopRight;
	m_points[FAR_BOTTOM_LEFT] = farBottomLeft;
	m_points[FAR_BOTTOM_RIGHT] = farBottomRight;

	m_points[NEAR_TOP_LEFT] = nearTopLeft;
	m_points[NEAR_TOP_RIGHT] = nearTopRight;
	m_points[NEAR_BOTTOM_LEFT] = nearBottomLeft;
	m_points[NEAR_BOTTOM_RIGHT] = nearBottomRight;

	// Calculate frustum planes
	m_planes[NEAR] = glm::vec4(cameraForward, nearDistance);
	m_planes[FAR] = glm::vec4(-cameraForward, farDistance);

	glm::vec3 leftPlaneNormal = glm::normalize(glm::cross(farBottomLeft - nearBottomLeft, farTopLeft - farBottomLeft));
	float leftPlaneOffset = glm::dot(leftPlaneNormal, nearBottomLeft);
	m_planes[LEFT] = glm::vec4(leftPlaneNormal, leftPlaneOffset);

	glm::vec3 rightPlaneNormal = glm::normalize(glm::cross(farTopRight - nearTopRight, farBottomRight - farTopRight));
	float rightPlaneOffset = glm::dot(rightPlaneNormal, nearTopRight);
	m_planes[RIGHT] = glm::vec4(rightPlaneNormal, rightPlaneOffset);

	glm::vec3 topPlaneNormal = glm::normalize(glm::cross(farTopLeft - nearTopLeft, farTopRight - farTopLeft));
	float topPlaneOffset = glm::dot(topPlaneNormal, nearTopLeft);
	m_planes[TOP] = glm::vec4(topPlaneNormal, leftPlaneOffset);

	glm::vec3 bottomPlaneNormal = glm::normalize(glm::cross(farBottomRight - nearBottomRight, farBottomLeft - farBottomRight));
	float bottomPlaneOffset = glm::dot(bottomPlaneNormal, nearBottomRight);
	m_planes[BOTTOM] = glm::vec4(bottomPlaneNormal, bottomPlaneOffset);
}

void Frustum::SetTransform(const glm::mat4 transform)
{
	m_transform = transform;
}

std::vector<glm::vec4> Frustum::GetPlanes()
{
	std::vector<glm::vec4> transformedPlanes(PLANE_COUNT);
	for (int i = 0; i < PLANE_COUNT; ++i)
	{
		transformedPlanes[i] = glm::transpose(glm::inverse(m_transform)) * m_planes[i];
	}
	return transformedPlanes;
}

std::vector<glm::vec3> Frustum::GetPoints()
{
	std::vector<glm::vec3> transformedPoints(POINT_COUNT);
	for (int i = 0; i < POINT_COUNT; ++i)
	{
		transformedPoints[i] = glm::vec3(m_transform * glm::vec4(m_points[i], 1.0f));
	}
	return transformedPoints;
}
