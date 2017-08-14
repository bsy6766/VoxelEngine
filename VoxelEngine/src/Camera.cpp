#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <iostream>

using namespace Voxel;
using namespace glm;

Camera* Camera::mainCamera = nullptr;

Camera::Camera()
	: position(0, 0, 0)
	, fovy(0)
	, aspect(0)
	, nears(0)
	, fars(0)
	, angle(0, 0, 0)
	, needUpdate(true)
	, curMatrix(mat4(1.0f))
{}

Camera::~Camera()
{
}

Camera* Camera::create(const vec3& position, const float fovy, const float nears, const float fars, const float aspect)
{
	Camera* newCamera = new Camera();

	newCamera->position = position;
	newCamera->nears = nears;
	newCamera->fars = fars;
	newCamera->aspect = aspect;
	newCamera->fovy = fovy;

	newCamera->curMatrix = newCamera->getMatrix();

	std::cout << "Creating camera. Fovy: " << fovy << ", Nears: " << nears << ", fars: " << fars << ", aspect: " << aspect << std::endl;

	return newCamera;
}

mat4 Camera::getProjection()
{
	auto mat = perspective(fovy, aspect, nears, fars);
	//mat[2] = { 0, 0, 0, nears };
	//mat[3] = { 0, 0, -1.0f, 0 };
	return mat;
}

mat4 Camera::getMatrix()
{
	if (needUpdate)
	{
		//curMatrix = getProjection() * getView();
		curMatrix = glm::translate(getProjection() * getOrientation(), -position);
		needUpdate = false;
		print();
	}

	return curMatrix;
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
	needUpdate = true;
}

void Camera::addPosition(const vec3 & distance)
{
	position += (distance * 1.0f);
	needUpdate = true;
}

void Camera::setAngle(const vec3 & angle)
{
	this->angle = angle;
	wrapAngle();
	needUpdate = true;

	//std::cout << "[Camera] angle = (" << this->angle.x << ", " << this->angle.y << ", " << this->angle.z << ")" << std::endl;
}

void Camera::addAngle(const vec3 & angle)
{
	this->angle += angle;
	wrapAngle();
	needUpdate = true;

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