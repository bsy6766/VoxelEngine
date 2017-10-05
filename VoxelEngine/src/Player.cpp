#include "Player.h"
#include <Camera.h>
#include <Utility.h>
#include <ProgramManager.h>
#include <Program.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/compatibility.hpp>

using namespace Voxel;

const float Player::MaxCameraDistance = 10.0f;
const float Player::DefaultJumpDistance = 3.5f;
const float Player::DefaultJumpCooldown = 0.1f;
const float Player::EyeHeight = 1.5f;

Player::Player()
	: position(0)
	, nextPosition(0)
	, rotation(0)
	, rotationTarget(0)
	, movementSpeed(6.0f)
	, rotationSpeed(15.0f)
	, fly(false)
	, mainCamera(Camera::mainCamera)
	, viewMatrix(mat4(1.0f))
	, dirMatrix(mat4(1.0f))
	, moved(false)
	, rotated(false)
	, direction(0)
	, rayRange(0)
	, lookingBlock(nullptr)
	, lookingFace(Cube::Face::NONE)
	, fallDuration(0)
	, fallDistance(0)
	, jumpDistance(Player::DefaultJumpDistance)
	, jumpCooldown(Player::DefaultJumpCooldown)
	, onGround(false)
	, cameraDistance(Player::MaxCameraDistance)
	, cameraDistanceTarget(Player::MaxCameraDistance)
	, viewMode(ViewMode::FIRST_PERSON_VIEW)
	// Debug
	, yLineVao(0)
	, rayVao(0)
	, boundingBoxVao(0)
{

}

Player::~Player()
{
	if (yLineVao)
	{
		glDeleteVertexArrays(1, &yLineVao);
	}

	if (rayVao)
	{
		glDeleteVertexArrays(1, &rayVao);
	}
}

void Voxel::Player::init()
{
	// Todo: load player info from save file
	this->direction = glm::vec3(0, 0, -1);
	// can reach up to 5 blocks from position
	this->rayRange = 5.0f;
}

void Voxel::Player::initYLine()
{
	// Generate vertex array object
	glGenVertexArrays(1, &yLineVao);
	// Bind it
	glBindVertexArray(yLineVao);

	// Generate buffer object
	GLuint yLineVbo;
	glGenBuffers(1, &yLineVbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, yLineVbo);

	GLfloat lines[] = {
		0, -100.0f, 0, 1, 0, 0, 1,
		0, 300.0f, 0, 1, 0, 0, 1,
	};

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
	// Enable vertices attrib
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_LINE);
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &yLineVbo);
}

void Voxel::Player::initRayLine()
{
	auto playerRayEnd = getRayEnd();
	// Generate vertex array object
	glGenVertexArrays(1, &rayVao);
	// Bind it
	glBindVertexArray(rayVao);

	// Generate buffer object
	GLuint rayVbo;
	glGenBuffers(1, &rayVbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, rayVbo);

	GLfloat ray[] = {
		0, 0, 0, 1, 0, 0, 1,
		0, 0, -1 * rayRange, 1, 0, 0, 1,
	};

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(ray), ray, GL_STATIC_DRAW);
	// Enable vertices attrib
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_LINE);
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");
	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &rayVbo);
}

void Voxel::Player::initBoundingBoxLine()
{	
	// Generate vertex array object
	glGenVertexArrays(1, &boundingBoxVao);
	// Bind it
	glBindVertexArray(boundingBoxVao);

	// Generate buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	auto min = glm::vec3(-0.4f, 0, -0.4f);
	auto max = glm::vec3(0.4f, 1.5f, 0.4f);

	// color = yellow
	GLfloat box[] = 
	{
		// bottom square
		min.x, min.y, min.z, 0, 1, 1, 1,
		max.x, min.y, min.z, 0, 1, 1, 1,
		max.x, min.y, min.z, 0, 1, 1, 1,
		max.x, min.y, max.z, 0, 1, 1, 1,
		max.x, min.y, max.z, 0, 1, 1, 1,
		min.x, min.y, max.z, 0, 1, 1, 1,
		min.x, min.y, max.z, 0, 1, 1, 1,
		min.x, min.y, min.z, 0, 1, 1, 1,
		
		// top square
		min.x, max.y, min.z, 0, 1, 1, 1,
		max.x, max.y, min.z, 0, 1, 1, 1,
		max.x, max.y, min.z, 0, 1, 1, 1,
		max.x, max.y, max.z, 0, 1, 1, 1,
		max.x, max.y, max.z, 0, 1, 1, 1,
		min.x, max.y, max.z, 0, 1, 1, 1,
		min.x, max.y, max.z, 0, 1, 1, 1,
		min.x, max.y, min.z, 0, 1, 1, 1,

		// 4 vertical lines
		min.x, min.y, min.z, 0, 1, 1, 1,
		min.x, max.y, min.z, 0, 1, 1, 1,
		max.x, min.y, min.z, 0, 1, 1, 1,
		max.x, max.y, min.z, 0, 1, 1, 1,
		min.x, min.y, max.z, 0, 1, 1, 1,
		min.x, max.y, max.z, 0, 1, 1, 1,
		max.x, min.y, max.z, 0, 1, 1, 1,
		max.x, max.y, max.z, 0, 1, 1, 1,
	};

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);

	// Enable vertices attrib
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_LINE);
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7* sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
}

void Player::moveFoward(const float delta)
{
	nextPosition += getMovedDistByKeyInput(-180.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
	//moved = true;
}

void Player::moveBackward(const float delta)
{
	nextPosition += getMovedDistByKeyInput(0, glm::vec3(0, 1, 0), movementSpeed * delta);
	//moved = true;
}

void Player::moveLeft(const float delta)
{
	nextPosition += getMovedDistByKeyInput(90.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
	//moved = true;
}

void Player::moveRight(const float delta)
{
	nextPosition += getMovedDistByKeyInput(-90.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
	//moved = true;
}

void Player::moveUp(const float delta)
{
	// move up
	nextPosition.y += movementSpeed * delta;
	//moved = true;
}

void Player::moveDown(const float delta)
{
	// move down
	nextPosition.y -= movementSpeed * delta;
	//moved = true;
}

void Player::jump(const float delta)
{
	onGround = false;

	if (jumpDistance > 0.0f)
	{
		float d = glm::lerp(0.0f, jumpDistance, delta * 10.0f);
		jumpDistance -= d;

		nextPosition.y += d;

		if (jumpDistance < 0.001f)
		{
			nextPosition.y += jumpDistance;
			jumpDistance = 0;
		}
	}
}

void Player::sneak()
{

}

void Voxel::Player::setFly(const bool mode)
{
	fly = mode;
}

glm::mat4 Voxel::Player::getViewMatrix()
{
	if (viewMode == ViewMode::FIRST_PERSON_VIEW)
	{
		return glm::translate(viewMatrix, -position) * glm::translate(glm::mat4(1.0f), glm::vec3(0, -Player::EyeHeight, 0));
	}
	else
	{
		return glm::rotate(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, 0, -cameraDistance)), glm::radians(-rotation.x), glm::vec3(1, 0, 0)), glm::radians(rotation.y), glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1), -position) * glm::translate(glm::mat4(1.0f), glm::vec3(0, -Player::EyeHeight, 0));
	}
}

/*
glm::mat4 Voxel::Player::getVP(const glm::mat4& projection)
{
	return glm::translate(projection * viewMatrix, -position);
}

glm::mat4 Voxel::Player::getDirVP(const glm::mat4 & projection)
{
	return glm::translate(projection * dirMatrix, -position);
}
*/

glm::mat4 Voxel::Player::getOrientation()
{
	return viewMatrix;
}

glm::mat4 Voxel::Player::getTranslationMat()
{
	return glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 Voxel::Player::getDirMatrix()
{
	return dirMatrix;
}

glm::mat4 Voxel::Player::getBillboardMatrix()
{
	auto billboardMatrix = mat4(1.0f);
	billboardMatrix = glm::rotate(billboardMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	billboardMatrix = glm::rotate(billboardMatrix, glm::radians(-rotation.y), glm::vec3(0, 1, 0));
	//billboardMatrix = glm::rotate(billboardMatrix, glm::radians(-rotation.z), glm::vec3(0, 0, 1));

	return billboardMatrix;
}

bool Voxel::Player::didMoveThisFrame()
{
	return moved;
}

bool Voxel::Player::didRotateThisFrame()
{
	return rotated;
}

void Voxel::Player::setMovementSpeed(float speed)
{
	if (speed < 0)
	{
		speed = 0;
	}
	this->movementSpeed = speed;
}

float Voxel::Player::getMovementSpeed()
{
	return movementSpeed;
}

float Voxel::Player::getFallDistance()
{
	return fallDistance;
}

void Voxel::Player::setFallDistance(const float dist)
{
	fallDistance = dist;
}

float Voxel::Player::getFallDuration()
{
	return fallDuration;
}

void Voxel::Player::setFallDuration(float time)
{
	if (time < 0.0f)
		time = 0.0f;

	fallDuration = time;
}

bool Voxel::Player::isOnGround()
{
	return onGround;
}

void Voxel::Player::setOnGround(const bool onGround)
{
	this->onGround = onGround;

	if (onGround)
	{
		// run cooldown;
		jumpCooldown = 0;
	}

	// take fall damage

	fallDistance = 0.0f;
	fallDuration = 0.0f;
}

bool Voxel::Player::isFlying()
{
	return fly;
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

void Voxel::Player::setViewMode(const int mode)
{
	if (mode == 0)
	{
		viewMode = ViewMode::FIRST_PERSON_VIEW;
	}
	else if (mode == 1)
	{
		viewMode = ViewMode::THIRD_PERSON_VIEW;
	}
}

Player::ViewMode Voxel::Player::getViewMode()
{
	return viewMode;
}

void Voxel::Player::zoomInCamera()
{
	this->cameraDistanceTarget -= 1.0f;
	if (this->cameraDistanceTarget < 1.0f)
	{
		this->cameraDistanceTarget = 1.0f;
	}
}

void Voxel::Player::zoomOutCamera()
{
	this->cameraDistanceTarget += 1.0f;
	if (this->cameraDistanceTarget > Player::MaxCameraDistance)
	{
		this->cameraDistanceTarget = Player::MaxCameraDistance;
	}
}

void Voxel::Player::setLookingBlock(Block* block, const Cube::Face& face)
{
	lookingBlock = block;
	lookingFace = face;
}

bool Voxel::Player::isLookingAtBlock()
{
	return lookingBlock != nullptr;
}

Block* Voxel::Player::getLookingBlock()
{
	return lookingBlock;
}

Cube::Face Voxel::Player::getLookingFace()
{
	return lookingFace;
}

Geometry::AABB Voxel::Player::getBoundingBox()
{
	return Geometry::AABB(glm::vec3(position.x, position.y + 0.75f, position.z), glm::vec3(0.8f, 1.5f, 0.8f));
}

Geometry::AABB Voxel::Player::getBoundingBox(const glm::vec3 & position)
{
	return Geometry::AABB(glm::vec3(position.x, position.y + 0.75f, position.z), glm::vec3(0.8f, 1.5f, 0.8f));
}

void Voxel::Player::jumpInstant(const float y)
{
	position.y += y;
	nextPosition.y += y;

	moved = true;
}

bool Voxel::Player::canJump()
{
	return (jumpDistance != 0);
}

void Voxel::Player::lockJump()
{
	// just make jump dist 0
	jumpDistance = 0;
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
	viewMatrix = glm::rotate(viewMatrix, glm::radians(-rotation.x), vec3(1, 0, 0));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), vec3(0, 1, 0));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), vec3(0, 0, 1));
}

void Voxel::Player::updateDirMatrix()
{
	dirMatrix = mat4(1.0f);
	dirMatrix = glm::rotate(dirMatrix, glm::radians(-rotation.y), glm::vec3(0, 1, 0));
	dirMatrix = glm::rotate(dirMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	dirMatrix = glm::rotate(dirMatrix, glm::radians(-rotation.z), glm::vec3(0, 0, 1));
}

void Voxel::Player::updateDirection()
{
	direction = vec3(dirMatrix * vec4(0, 0, -1, 1));
	//std::cout << "Updating player view direction (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << std::endl;
}

void Voxel::Player::update(const float delta)
{
	if (rotation.x != rotationTarget.x)
	{
		rotation.x = glm::lerp(rotation.x, rotationTarget.x, 20.0f * delta);

		if (rotation.x >= 360.0f || rotation.x < 0)
		{	
			// angle x range: 0 ~ 360
			wrapAngle(rotation.x);
			wrapAngle(rotationTarget.x);
		}

		// Angle x range: 0~90, 360 ~ 270
		wrapAngleX();

		if (glm::abs(glm::distance(rotation.x, rotationTarget.x)) <= 0.01f)
		{
			rotation.x = rotationTarget.x;
		}

		updateViewMatrix();
		updateDirMatrix();
		updateDirection();
		rotated = true;
	}
	else
	{
		rotated = false;
	}

	if (rotation.y != rotationTarget.y)
	{
		rotation.y = glm::lerp(rotation.y, rotationTarget.y, 20.0f * delta);

		if (rotation.y >= 360.0f || rotation.y < 0)
		{
			wrapAngle(rotation.y);
			wrapAngle(rotationTarget.y);
		}

		if (glm::abs(glm::distance(rotation.y, rotationTarget.y)) <= 0.01f)
		{
			rotation.y = rotationTarget.y;
		}

		updateViewMatrix();
		updateDirMatrix();
		updateDirection();
		rotated = true;
	}
	else
	{
		rotated = false;
	}

	if (cameraDistance != cameraDistanceTarget)
	{
		cameraDistance = glm::lerp(cameraDistance, cameraDistanceTarget, 10.0f * delta);
		if (glm::abs(cameraDistanceTarget - cameraDistance) < 0.01f)
		{
			cameraDistance = cameraDistanceTarget;
		}
	}

	if (jumpCooldown < Player::DefaultJumpCooldown)
	{
		jumpCooldown += delta;

		if (jumpCooldown >= Player::DefaultJumpCooldown)
		{
			jumpCooldown = Player::DefaultJumpCooldown;
			// refill jump distance
			jumpDistance = Player::DefaultJumpDistance;
		}
	}
}

void Voxel::Player::updateMovement(const float delta)
{
	if (position != nextPosition)
	{
		position = glm::lerp(position, nextPosition, 20.0f * delta);
		//std::cout << "p: " << Utility::Log::vec3ToStr(position) << "lerp np:" << Utility::Log::vec3ToStr(nextPosition) <<  std::endl;
		if (glm::abs(glm::distance(position, nextPosition)) <= 0.01f)
		{
			position = nextPosition;
		}

		//position = nextPosition;

		moved = true;
	}
	else
	{
		moved = false;
	}
}

void Voxel::Player::renderDebugLines(Program* lineProgram)
{
	if (yLineVao)
	{
		glBindVertexArray(yLineVao);

		lineProgram->setUniformMat4("modelMat", glm::translate(mat4(1.0f), position));
		glDrawArrays(GL_LINES, 0, 2);
	}

	if (rayVao)
	{
		glBindVertexArray(rayVao);

		glm::mat4 rayMat = mat4(1.0f);
		rayMat = glm::translate(rayMat, getEyePosition());
		rayMat = glm::rotate(rayMat, glm::radians(-rotation.y), glm::vec3(0, 1, 0));
		rayMat = glm::rotate(rayMat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		rayMat = glm::rotate(rayMat, glm::radians(-rotation.z), glm::vec3(0, 0, 1));

		lineProgram->setUniformMat4("modelMat", rayMat);
		glDrawArrays(GL_LINES, 0, 2);
	}

	if (boundingBoxVao)
	{
		glBindVertexArray(boundingBoxVao);
		lineProgram->setUniformMat4("modelMat", glm::translate(mat4(1.0f), position));
		glDrawArrays(GL_LINES, 0, 24);
	}
}

glm::vec3 Player::getPosition()
{
	return position;
}

void Voxel::Player::setPosition(const glm::vec3 & newPosition, const bool smoothMovement)
{
	if (smoothMovement)
	{
		nextPosition = newPosition;
	}
	else
	{
		position = newPosition;
		nextPosition = newPosition;

		moved = true;
	}
}

void Voxel::Player::setNextPosition(const glm::vec3 & nextPosition)
{
	this->nextPosition = nextPosition;
}

void Voxel::Player::applyNextPosition()
{
	position = nextPosition;
}

glm::vec3 Voxel::Player::getNextPosition()
{
	return nextPosition;
}

glm::vec3 Voxel::Player::getEyePosition()
{
	return glm::vec3(position.x, position.y + Player::EyeHeight, position.z);
}

void Voxel::Player::addRotationX(const float x)
{
	rotationTarget.x += x * rotationSpeed;
	wrapNextAngleX();
}
	
void Voxel::Player::addRotationY(const float y)
{
	rotationTarget.y += y * rotationSpeed;
}

void Player::wrapAngle(float& axis)
{
	if (axis < 0) axis += 360.0f;
	else if (axis >= 360.0f) axis -= 360.0f;
}

void Voxel::Player::wrapAngle()
{
	wrapAngle(rotation.x);
	wrapAngle(rotation.y);
	wrapAngle(rotation.z);
}

void Voxel::Player::wrapNextAngleX()
{
	if (rotationTarget.x < 180.0f)
	{
		if (rotationTarget.x < -90.0f)
		{
			rotationTarget.x = -90.0f;
		}
		else if (rotationTarget.x > 90.0f)
		{
			rotationTarget.x = 90.0f;
		}
	}
	else if (rotationTarget.x >= 180.0f)
	{
		if (rotationTarget.x < 270.0f)
		{
			rotationTarget.x = 270.0f;
		}
		else if (rotationTarget.x > 450.0f)
		{
			rotationTarget.x = 450.0f;
		}
	}
}

void Voxel::Player::wrapAngleX()
{
	if (rotation.x < 180.0f)
	{
		if (rotation.x > 90.0f)
		{
			rotation.x = 90.0f;
		}
	}
	else if (rotation.x >= 180.0f)
	{
		if (rotation.x < 270.0f)
		{
			rotation.x = 270.0f;
		}
	}
}


void Voxel::Player::setRotation(const glm::vec3 & newRotation)
{
	rotation = newRotation;

	wrapAngle();

	updateViewMatrix();
	updateDirMatrix();
	updateDirection();
	rotated = true;
}

glm::vec3 Voxel::Player::getRotation()
{
	return rotation; 
}
