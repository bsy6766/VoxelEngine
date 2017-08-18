#include "Player.h"
#include <Camera.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace Voxel;

Player::Player()
	: position(glm::vec3(0))
	, rotation(glm::vec3(0))
	, movementSpeed(15.0f)
	, rotationSpeed(15.0f)
	, fly(false)
	, mainCamera(Camera::mainCamera)
	, viewMatrix(mat4(1.0f))
	, moved(false)
	, rotated(false)
	, direction(0)
	, rayRange(0)
	, lookingBlock(nullptr)
{

}

Player::~Player()
{

}

void Player::moveFoward(const float delta)
{
	position += getMovedDistByKeyInput(-180.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
	moved = true;
}

void Player::moveBackward(const float delta)
{
	position += getMovedDistByKeyInput(0, glm::vec3(0, 1, 0), movementSpeed * delta);
	moved = true;
}

void Player::moveLeft(const float delta)
{
	position += getMovedDistByKeyInput(90.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
	moved = true;
}

void Player::moveRight(const float delta)
{
	position += getMovedDistByKeyInput(-90.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
	moved = true;
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
	moved = true;
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
	moved = true;
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

glm::mat4 Voxel::Player::getOrientation()
{
	return viewMatrix;
}

bool Voxel::Player::didMoveThisFrame()
{
	return moved;
}

bool Voxel::Player::didRotateThisFrame()
{
	return rotated;
}

glm::vec3 Voxel::Player::getDirection()
{
	return direction;
}

float Voxel::Player::getRange()
{
	return rayRange;
}

glm::vec3 Voxel::Player::getRayEnd()
{
	return position + (direction * rayRange);
}

void Voxel::Player::setLookingBlock(Block* block)
{
	lookingBlock = block;
}

bool Voxel::Player::isLookingAtBlock()
{
	return lookingBlock != nullptr;
}

Block* Voxel::Player::getLookingBlock()
{
	return lookingBlock;
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

void Voxel::Player::updateDirection()
{
	auto defaultDiretion = glm::vec3(0, 0, -1);


	glm::mat4 rayMat = mat4(1.0f);
	//rayMat = glm::translate(rayMat, position);
	rayMat = glm::rotate(rayMat, glm::radians(-rotation.y), glm::vec3(0, 1, 0));
	rayMat = glm::rotate(rayMat, glm::radians(-rotation.x), glm::vec3(1, 0, 0));
	rayMat = glm::rotate(rayMat, glm::radians(-rotation.z), glm::vec3(0, 0, 1));


	direction = vec3(rayMat * vec4(defaultDiretion, 1));
	// x direction in player's view and world is opposite
	//direction.x *= -1.0f;
	// direction = glm::normalize(direction);
	//std::cout << "Updating player view direction (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;
}

void Voxel::Player::init(const glm::vec3 & position)
{
	// Todo: load player info from save file
	this->position = position;
	this->direction = glm::vec3(0, 0, -1);
	// can reach up to 5 blocks from position
	this->rayRange = 5.0f;
}

void Voxel::Player::update()
{
	moved = false;
	rotated = false;
}

glm::vec3 Player::getPosition()
{
	return position;
}

void Voxel::Player::setPosition(const glm::vec3 & newPosition)
{
	position = newPosition;
	moved = true;
}

void Voxel::Player::addRotationX(const float x)
{
	rotation.x += x * rotationSpeed;
	wrapAngle(rotation.x);
	updateViewMatrix();
	updateDirection();
	rotated = true;
}

void Voxel::Player::addRotationY(const float y)
{
	rotation.y += y * rotationSpeed;
	wrapAngle(rotation.y);
	updateViewMatrix();
	updateDirection();
	rotated = true;
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
	updateDirection();
	rotated = true;
}

glm::vec3 Voxel::Player::getRotation()
{
	return rotation; 
}
