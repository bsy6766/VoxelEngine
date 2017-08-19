#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <Frustum.h>

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
	, needUpdate(true)
	, curMatrix(mat4(1.0f))
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

	//newCamera->initFrustumPlanes();

	newCamera->curMatrix = newCamera->getMatrix();

	newCamera->frustum = new Frustum();

	newCamera->screenSpacePos = glm::vec3(0, 0, (screenHeight * 0.5f) / tan(glm::radians(fovy * 0.5f)));

	std::cout << "[Camera] Creating camera. Fovy: " << fovy << ", Fovx: " << newCamera->fovx << ", Nears: " << nears << ", fars: " << fars << ", aspect: " << newCamera->aspect << std::endl;
	std::cout << "[Camera] Camera position (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	std::cout << "[Camera] Camera rotation (" << newCamera->angle.x << ", " << newCamera->angle.y << ", " << newCamera->angle.z << ")" << std::endl;
	std::cout << "[Camera] Camera screen space (" << newCamera->screenSpacePos.x << ", " << newCamera->screenSpacePos.y << ", " << newCamera->screenSpacePos.z << ")" << std::endl;
	std::cout << std::endl;

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
	glm::vec2 b = glm::vec2(tanValue * nears, nears);
	glm::vec2 a = b;
	a.x *= -1.0f;

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

	nearNormal = glm::vec2(0, 1);
	farNormal = glm::vec2(0, -1);

	float left_dx = c.x - a.x;
	float left_dy = c.y - a.y;
	
	leftNormal = glm::normalize(glm::vec2(left_dy, -left_dx));

	float right_dx = d.x - b.x;
	float right_dy = d.y - b.y;

	rightNormal = glm::normalize(glm::vec2(-right_dy, right_dx));

	std::cout << "[Camera] Near normal: (" << nearNormal.x << ", " << nearNormal.y << ")" << std::endl;
	std::cout << "[Camera] Far normal: (" << farNormal.x << ", " << farNormal.y << ")" << std::endl;
	std::cout << "[Camera] Left normal: (" << leftNormal.x << ", " << leftNormal.y << ")" << std::endl;
	std::cout << "[Camera] Right normal: (" << rightNormal.x << ", " << rightNormal.y << ")" << std::endl;
}

void Voxel::Camera::updateFrustum(const glm::vec3& playerPosition, const glm::mat4& playerOrientation)
{
	auto MVP = glm::translate(perspective(fovy, aspect, nears, 256.0f) * playerOrientation, -playerPosition);
	frustum->update(MVP);
}

void Voxel::Camera::updateFrustumPlane(const vec3 & playerPosition, const vec3 & playerRotation)
{
	auto yAngleShift = playerRotation.y - 180.0f;
	//yAngleShift = -52.2673f;

	//std::cout << "py = " << playerRotation.y << ", shift = " << -yAngleShift << std::endl;

	//auto eOrigin = glm::vec3(0, 0, fars);

	auto radian = glm::radians(-yAngleShift);

	/*
	glm::vec3 e = glm::rotateY(eOrigin, radian);
	e.x += playerPosition.x;
	e.y += playerPosition.y;
	*/

	auto tanValue = tan(glm::radians(fovx * 0.5f));
	glm::vec3 b = glm::vec3(tanValue * nears, 0, nears);
	glm::vec3 a = b;
	a.x *= -1.0f;

	glm::vec3 d = glm::vec3(tan(glm::radians(fovx * 0.5f)) * fars, 0, fars);
	glm::vec3 c = d;
	c.x *= -1.0f;

	glm::vec3 aa = glm::rotateY(a, radian);
	glm::vec3 bb = glm::rotateY(b, radian);
	glm::vec3 cc = glm::rotateY(c, radian);
	glm::vec3 dd = glm::rotateY(d, radian);
	
	farPlane = glm::vec4(glm::vec2(cc.x, cc.z), glm::vec2(dd.x, dd.z));
	nearPlane = glm::vec4(glm::vec2(aa.x, aa.z), glm::vec2(bb.x, bb.z));
	leftPlane = glm::vec4(glm::vec2(aa.x, aa.z), glm::vec2(cc.x, cc.z));
	rightPlane = glm::vec4(glm::vec2(bb.x, bb.z), glm::vec2(dd.x, dd.z));

	nearNormal = glm::vec2(0, 1);
	farNormal = glm::vec2(0, -1);

	float left_dx = c.x - a.x;
	float left_dy = c.z - a.z;

	leftNormal = glm::normalize(glm::vec2(left_dy, -left_dx));

	float right_dx = d.x - b.x;
	float right_dy = d.z - b.z;

	rightNormal = glm::normalize(glm::vec2(-right_dy, right_dx));

	glm::vec3 nearN3 = glm::rotateY(glm::vec3(nearNormal.x, 0, nearNormal.y), radian);
	nearNormal = glm::vec2(nearN3.x, nearN3.z);

	glm::vec3 farN3 = glm::rotateY(glm::vec3(farNormal.x, 0, farNormal.y), radian);
	farNormal = glm::vec2(farN3.x, farN3.z);

	glm::vec3 leftN3 = glm::rotateY(glm::vec3(leftNormal.x, 0, leftNormal.y), radian);
	leftNormal = glm::vec2(leftN3.x, leftN3.z);

	glm::vec3 rightN3 = glm::rotateY(glm::vec3(rightNormal.x, 0, rightNormal.y), radian);
	rightNormal = glm::vec2(rightN3.x, rightN3.z);

	/*
	std::cout << "[Camera] Frustum info" << std::endl;
	std::cout << "[Camera] Near plane: (" << nearPlane.x << ", " << nearPlane.y << "), (" << nearPlane.z << ", " << nearPlane.w << ")" << std::endl;
	std::cout << "[Camera] Far plane: (" << farPlane.x << ", " << farPlane.y << "), (" << farPlane.z << ", " << farPlane.w << ")" << std::endl;
	std::cout << "[Camera] Left plane: (" << leftPlane.x << ", " << leftPlane.y << "), (" << leftPlane.z << ", " << leftPlane.w << ")" << std::endl;
	std::cout << "[Camera] Right plane: (" << rightPlane.x << ", " << rightPlane.y << "), (" << rightPlane.z << ", " << rightPlane.w << ")" << std::endl;
	std::cout << "[Camera] Near normal: (" << nearNormal.x << ", " << nearNormal.y << ")" << std::endl;
	std::cout << "[Camera] Far normal: (" << farNormal.x << ", " << farNormal.y << ")" << std::endl;
	std::cout << "[Camera] Left normal: (" << leftNormal.x << ", " << leftNormal.y << ")" << std::endl;
	std::cout << "[Camera] Right normal: (" << rightNormal.x << ", " << rightNormal.y << ")" << std::endl;
	*/

}

Frustum * Voxel::Camera::getFrustum()
{
	return frustum;
}

glm::vec3 Voxel::Camera::getScreenSpacePos()
{
	return screenSpacePos;
}

void Voxel::Camera::updateScreenSizeAndAspect(const float screenWidth, const float screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	aspect = screenWidth / screenHeight;
	screenSpacePos = glm::vec3(0, 0, (screenHeight * 0.5f) / tan(glm::radians(fovy * 0.5f)));
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