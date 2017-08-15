#include "Player.h"
#include <Camera.h>
#include <glm/gtx/transform.hpp>

using namespace Voxel;

Player::Player()
	: position(glm::vec3(0))
	, rotation(glm::vec3(0))
	, movementSpeed(15.0f)
	, rotationSpeed(170.0f)
	, fly(false)
	, mainCamera(Camera::mainCamera)
	, viewMatrix(mat4(1.0f))
{

}

Player::~Player()
{

}

void Player::moveFoward(const float delta)
{
	position += getMovedDistByKeyInput(-180.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
}

void Player::moveBackward(const float delta)
{
	position += getMovedDistByKeyInput(0, glm::vec3(0, 1, 0), movementSpeed * delta);
}

void Player::moveLeft(const float delta)
{
	position += getMovedDistByKeyInput(90.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
}

void Player::moveRight(const float delta)
{
	position += getMovedDistByKeyInput(-90.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
}

void Player::moveUp(const float delta)
{
	if (fly)
	{
		// move up
		position.y += movementSpeed * delta;
	}
	else
	{
		// jump
		jump();
	}
}

void Player::moveDown(const float delta)
{
	if (fly)
	{
		// move down
		position.y -= movementSpeed * delta;
	}
	else
	{
		// sneak
		sneak();
	}
}

void Player::jump()
{

}

void Player::sneak()
{

}

void Voxel::Player::setFly(const bool mode)
{
	fly = mode;
}

glm::mat4 Voxel::Player::getVP(const glm::mat4& projection)
{
	return glm::translate(projection * viewMatrix, -position);
}

glm::vec3 Voxel::Player::getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance)
{
	float angle = 0;
	if (axis.y == 1.0f)
	{
		angle = rotation.y;
	}

	angle += angleMod;

	if (angle < 0) angle += 360.0f;
	else if (angle >= 360.0f) angle -= 360.0f;

	auto rotateMat = glm::rotate(mat4(1.0f), glm::radians(angle), axis);
	auto movedDist = glm::inverse(rotateMat) * glm::vec4(0, 0, distance, 1);

	return movedDist;
}

void Voxel::Player::updateViewMatrix()
{
	viewMatrix = mat4(1.0f);
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), vec3(1, 0, 0));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), vec3(0, 1, 0));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), vec3(0, 0, 1));
}

void Voxel::Player::init(const glm::vec3 & position)
{
	this->position = position;
}

glm::vec3 Player::getPosition()
{
	return position;
}

void Voxel::Player::setPosition(const glm::vec3 & newPosition)
{
	position = newPosition;
}

void Voxel::Player::addRotationX(const float x)
{
	rotation.x += x * rotationSpeed;
	wrapAngle(rotation.x);
	updateViewMatrix();
}

void Voxel::Player::addRotationY(const float y)
{
	rotation.y += y * rotationSpeed;
	wrapAngle(rotation.y);
	updateViewMatrix();
}

void Player::wrapAngle(float& axis)
{
	if (axis < 0) axis += 360.0f;
	else if (axis >= 360.0f) axis -= 360.0f;
}

void Voxel::Player::setRotation(const glm::vec3 & newRotation)
{
	rotation = newRotation;
	updateViewMatrix();
}

glm::vec3 Voxel::Player::getRotation()
{
	return rotation; 
}
