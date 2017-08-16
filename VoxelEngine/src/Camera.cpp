#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

using namespace Voxel;
using namespace glm;

Camera* Camera::mainCamera = nullptr;

Camera::Camera()
	: position(0, 0, 0)
	, fovy(0)
	, fovx(0)
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

	// Refernce: http://wiki.panotools.org/Field_of_View
	newCamera->fovx = glm::degrees(2.0f * atan(tan(glm::radians(fovy * 0.5f)) * aspect));

	newCamera->initFrustumPlanes();

	newCamera->curMatrix = newCamera->getMatrix();

	std::cout << "[Camera] Creating camera. Fovy: " << fovy << ", Fovx: " << newCamera->fovx << ", Nears: " << nears << ", fars: " << fars << ", aspect: " << aspect << std::endl;

	return newCamera;
}

void Voxel::Camera::initFrustumPlanes()
{
	// use fovx to calculate points.
	/*
								*
							   /|\
							  / | \
							a/__|__\b
							/   |   \
						   /	|    \
						  / 	|     \
						c/______|______\d
								|e
	*/

	auto tanValue = tan(glm::radians(fovx * 0.5f));
	glm::vec2 a = glm::vec2(tanValue * nears, nears);
	glm::vec2 b = a;
	b.x *= -1.0f;

	glm::vec2 d = glm::vec2(tan(glm::radians(fovx * 0.5f)) * fars, fars);
	glm::vec2 c = d;
	c.x *= -1.0f;

	std::cout << "[Camera] Frustum info" << std::endl;
	std::cout << "[Camera] Near plane: (" << a.x << ", " << a.y << "), (" << b.x << ", " << b.y << ")" << std::endl;
	std::cout << "[Camera] Far plane: (" << c.x << ", " << c.y << "), (" << d.x << ", " << d.y << ")" << std::endl;
	std::cout << "[Camera] Left plane: (" << a.x << ", " << a.y << "), (" << c.x << ", " << c.y << ")" << std::endl;
	std::cout << "[Camera] Right plane: (" << b.x << ", " << b.y << "), (" << d.x << ", " << d.y << ")" << std::endl;

	farPlane = glm::vec4(c, d);
	nearPlane = glm::vec4(a, b);
	leftPlane = glm::vec4(a, c);
	rightPlane = glm::vec4(b, d);
}

void Voxel::Camera::updateFrustumPlane(const vec3 & playerPosition, const vec3 & playerRotation)
{
	auto yAngleShift = playerRotation.y - 180.0f;

	auto eOrigin = glm::vec3(playerPosition.x, playerPosition.z, fars);

	glm::vec3 e = glm::rotateY(eOrigin, glm::radians(-yAngleShift));

	auto tanValue = tan(glm::radians(fovx * 0.5f));
	glm::vec3 a = glm::vec3(tanValue * nears, 0, nears);
	glm::vec3 b = a;
	b.x *= -1.0f;

	glm::vec3 d = glm::vec3(tan(glm::radians(fovx * 0.5f)) * fars, 0, fars);
	glm::vec3 c = d;
	c.x *= -1.0f;

	glm::vec3 aa = glm::rotateY(a, glm::radians(-yAngleShift));
	glm::vec3 bb = glm::rotateY(b, glm::radians(-yAngleShift));
	glm::vec3 cc = glm::rotateY(c, glm::radians(-yAngleShift));
	glm::vec3 dd = glm::rotateY(d, glm::radians(-yAngleShift));
	
	farPlane = glm::vec4(glm::vec2(cc.x, cc.z), glm::vec2(dd.x, dd.z));
	nearPlane = glm::vec4(glm::vec2(aa.x, aa.z), glm::vec2(bb.x, bb.z));
	leftPlane = glm::vec4(glm::vec2(aa.x, aa.z), glm::vec2(cc.x, cc.z));
	rightPlane = glm::vec4(glm::vec2(bb.x, bb.z), glm::vec2(dd.x, dd.z));

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
		//print();
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