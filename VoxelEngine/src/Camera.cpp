#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <Frustum.h>
#include <ChunkUtil.h>

using namespace Voxel;
using namespace glm;

Camera* Camera::mainCamera = nullptr;

Camera::Camera()
	: position(0, 0, 0)
	, fovy(0)
	, fovx(0)
	, screenWidth(0)
	, screenHeight(0)
	, aspect(0)
	, nears(0)
	, fars(0)
	, angle(0, 0, 0)
	, frustum(nullptr)
	, screenSpacePos(0)
{}

Camera::~Camera()
{
	if (frustum)
	{
		delete frustum;
	}
}

Camera* Camera::create(const vec3& position, const float fovy, const float nears, const float fars, const float screenWidth, const float screenHeight)
{
	Camera* newCamera = new Camera();

	newCamera->position = position;
	newCamera->nears = nears;
	newCamera->fars = fars;
	newCamera->screenWidth = screenWidth;
	newCamera->screenHeight = screenHeight;
	newCamera->aspect = screenWidth / screenHeight;
	newCamera->fovy = fovy;

	// Refernce: http://wiki.panotools.org/Field_of_View
	newCamera->fovx = glm::degrees(2.0f * atan(tan(glm::radians(fovy * 0.5f)) * newCamera->aspect));
	
	newCamera->frustum = new Frustum();

	newCamera->screenSpacePos = glm::vec3(0, 0, (screenHeight * 0.5f) / tan(glm::radians(fovy * 0.5f)));

	std::cout << "[Camera] Creating camera. Fovy: " << fovy << ", Fovx: " << newCamera->fovx << ", Nears: " << nears << ", fars: " << fars << ", aspect: " << newCamera->aspect << std::endl;
	std::cout << "[Camera] Camera position (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	std::cout << "[Camera] Camera rotation (" << newCamera->angle.x << ", " << newCamera->angle.y << ", " << newCamera->angle.z << ")" << std::endl;
	std::cout << "[Camera] Camera screen space (" << newCamera->screenSpacePos.x << ", " << newCamera->screenSpacePos.y << ", " << newCamera->screenSpacePos.z << ")" << std::endl;
	std::cout << std::endl;

	return newCamera;
}

void Voxel::Camera::updateFrustum(const glm::vec3& playerPosition, const glm::mat4& playerOrientation, const int renderDistance)
{
	auto MVP = glm::translate(perspective(glm::radians(fovy), aspect, nears, static_cast<float>(renderDistance * 2.0f) * Constant::CHUNK_BORDER_SIZE) * playerOrientation, -playerPosition);
	frustum->update(MVP);
}

Frustum * Voxel::Camera::getFrustum()
{
	return frustum;
}

void Voxel::Camera::updateScreenSizeAndAspect(const float screenWidth, const float screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	aspect = screenWidth / screenHeight;
	screenSpacePos = glm::vec3(0, 0, (screenHeight * 0.5f) / tan(glm::radians(fovy * 0.5f)));
}

void Voxel::Camera::setFovy(const float fovy)
{
	this->fovy = fovy;
}

float Voxel::Camera::getFovy()
{
	return fovy;
}

glm::mat4 Voxel::Camera::getScreenSpaceMatrix()
{
	return glm::translate(glm::mat4(1.0f), -screenSpacePos);
}

void Voxel::Camera::initDebugFrustumLines()
{
	this->frustum->initDebugLines(fovy, fovx, nears, 8.0f * 16.0f);
}

mat4 Camera::getProjection()
{
	return perspective(glm::radians(fovy), aspect, nears, fars);
}

mat4 Voxel::Camera::getProjection(const float fovy)
{
	return perspective(glm::radians(fovy), aspect, nears, fars);
}

mat4 Camera::getView()
{
	// View matrix defined the position (location and orientation) of the camera
	//return lookAt(position, vec3(0), vec3(0, 1, 0));
	return glm::translate(getOrientation(), -position);
}

mat4 Camera::getOrientation()
{
	mat4 orientation = mat4(1.0f);
	orientation = glm::rotate(orientation, glm::radians(angle.x), vec3(1, 0, 0));
	orientation = glm::rotate(orientation, glm::radians(angle.y), vec3(0, 1, 0));
	orientation = glm::rotate(orientation, glm::radians(angle.z), vec3(0, 0, 1));
	return orientation;
}

vec3 Camera::getPosition()
{
	return position;
}

void Camera::setPosition(const vec3& position)
{
	this->position = position;
}

void Camera::addPosition(const vec3 & distance)
{
	position += (distance * 1.0f);
}

void Camera::setAngle(const vec3 & angle)
{
	this->angle = angle;
	wrapAngle();

	//std::cout << "[Camera] angle = (" << this->angle.x << ", " << this->angle.y << ", " << this->angle.z << ")" << std::endl;
}

void Camera::addAngle(const vec3 & angle)
{
	this->angle += angle;
	wrapAngle();

	//std::cout << "[Camera] angle = (" << this->angle.x << ", " << this->angle.y << ", " << this->angle.z << ")" << std::endl;
}

float Voxel::Camera::getAngleY()
{
	return angle.y;
}

void Camera::wrapAngle()
{
	wrapAngle(angle.x);
	wrapAngle(angle.y);
	wrapAngle(angle.z);
}

void Camera::wrapAngle(float& axis)
{
	if (axis >= 360.0f)
	{
		axis -= 360.0f;
	}
	else if (axis < 0.0f)
	{
		axis += 360.0f;
	}
}

void Camera::print()
{
	std::cout << "[Camera] position = (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")" << std::endl;
	std::cout << "[Camera] angle = (" << this->angle.x << ", " << this->angle.y << ", " << this->angle.z << ")" << std::endl;
}