#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <iostream>

using namespace Voxel;
using namespace glm;

Camera::Camera()
	: position(0, 0, 0)
	, fovy(0)
	, aspect(0)
	, nears(0)
	, fars(0)
	, angle(0, 0, 0)
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
	return perspective(fovy, aspect, nears, fars);
}

mat4 Camera::getMatrix()
{
	return getProjection() * getView();
}

mat4 Camera::getView()
{
	return translate(getOrientation(), position);
}

mat4 Camera::getOrientation()
{
	mat4 orientation = mat4(1.0f);
	orientation = glm::rotate(orientation, angle.x, vec3(1, 0, 0));
	orientation = glm::rotate(orientation, angle.y, vec3(0, 1, 0));
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
