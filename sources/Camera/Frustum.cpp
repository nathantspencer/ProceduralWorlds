#include "Frustum.h"

#include <Common/Math/Constants.h>


Frustum::Frustum(float nearDistance, float farDistance, float fov, float aspect)
{
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

	m_points.push_back(farTopLeft);
	m_points.push_back(farTopRight);
	m_points.push_back(farBottomLeft);
	m_points.push_back(farBottomRight);

	m_points.push_back(nearTopLeft);
	m_points.push_back(nearTopRight);
	m_points.push_back(nearBottomLeft);
	m_points.push_back(nearBottomRight);

	// Calculate frustum planes
	glm::vec4 nearPlane = glm::vec4(cameraForward, nearDistance);
	glm::vec4 farPlane = glm::vec4(-cameraForward, farDistance);

	glm::vec3 leftPlaneNormal = glm::normalize(glm::cross(farBottomLeft - nearBottomLeft, farTopLeft - farBottomLeft));
	float leftPlaneOffset = glm::dot(leftPlaneNormal, nearBottomLeft);
	glm::vec4 leftPlane = glm::vec4(leftPlaneNormal, leftPlaneOffset);

	glm::vec3 rightPlaneNormal = glm::normalize(glm::cross(farTopRight - nearTopRight, farBottomRight - farTopRight));
	float rightPlaneOffset = glm::dot(rightPlaneNormal, nearTopRight);
	glm::vec4 rightPlane = glm::vec4(rightPlaneNormal, rightPlaneOffset);

	glm::vec3 topPlaneNormal = glm::normalize(glm::cross(farTopLeft - nearTopLeft, farTopRight - farTopLeft));
	float topPlaneOffset = glm::dot(topPlaneNormal, nearTopLeft);
	glm::vec4 topPlane = glm::vec4(topPlaneNormal, leftPlaneOffset);

	glm::vec3 bottomPlaneNormal = glm::normalize(glm::cross(farBottomRight - nearBottomRight, farBottomLeft - farBottomRight));
	float bottomPlaneOffset = glm::dot(bottomPlaneNormal, nearBottomRight);
	glm::vec4 bottomPlane = glm::vec4(bottomPlaneNormal, bottomPlaneOffset);

	glm::vec3 bottomPlaneNormal = glm::normalize(glm::cross(farBottomRight - nearBottomRight, farBottomLeft - farBottomRight));
	float bottomPlaneOffset = glm::dot(bottomPlaneNormal, nearBottomRight);
	glm::vec4 bottomPlane = glm::vec4(bottomPlaneNormal, bottomPlaneOffset);

	m_planes.push_back(nearPlane);
	m_planes.push_back(farPlane);
	m_planes.push_back(leftPlane);
	m_planes.push_back(rightPlane);
	m_planes.push_back(topPlane);
	m_planes.push_back(bottomPlane);
}

void Frustum::SetTransform(const glm::mat4 transform)
{
	m_transform = transform;
}

std::vector<glm::vec4> Frustum::GetPlanes()
{
	// TODO
	return std::vector<glm::vec4>();
}

std::vector<glm::vec3> Frustum::GetPoints()
{
	// TODO
	return std::vector<glm::vec3>();
}
