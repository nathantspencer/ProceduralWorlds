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

	m_transformedPoints = m_points;
	m_transformedPlanes = m_planes;
}

void Frustum::SetTransform(const glm::mat4& transform)
{
	m_transform = transform;

	for (int i = 0; i < POINT_COUNT; ++i)
	{
		m_transformedPoints[i] = glm::vec3(m_transform * glm::vec4(m_points[i], 1.0f));
	}

	for (int i = 0; i < PLANE_COUNT; ++i)
	{
		m_transformedPlanes[i] = glm::transpose(glm::inverse(m_transform)) * m_planes[i];
	}
}

std::vector<glm::vec4> Frustum::GetPlanes() const
{
	return m_transformedPlanes;
}

std::vector<glm::vec3> Frustum::GetPoints() const
{
	return m_transformedPoints;
}

bool Frustum::IsBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const
{
	for (int i = 0; i < PLANE_COUNT; i++)
	{
		int outsidePlaneCount = 0;
		for (int j = 0; j < 8; ++j)
		{
			auto testPoint = glm::vec4(j % 2 ? min.x : max.x, j % 4 ? min.y : max.y, j % 8 ? min.z : max.z, 1.0f);
			outsidePlaneCount += glm::dot(m_transformedPlanes[i], testPoint) < 0.0f ? 1 : 0;
		}
		if (outsidePlaneCount == 8)
		{
			return false;
		}
	}
	return true;
	// TODO: check against points as well, see [https://www.iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm]
}
