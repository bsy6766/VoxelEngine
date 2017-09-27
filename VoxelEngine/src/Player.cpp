#include "Player.h"
#include <Camera.h>
#include <ProgramManager.h>
#include <Program.h>
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
	, dirMatrix(mat4(1.0f))
	, moved(false)
	, rotated(false)
	, direction(0)
	, rayRange(0)
	, lookingBlock(nullptr)
	, lookingFace(Cube::Face::NONE)
	// Debug
	, yLineVao(0)
	, rayVao(0)
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

void Voxel::Player::init(const glm::vec3 & position)
{
	// Todo: load player info from save file
	this->position = position;
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
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &rayVbo);
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

glm::mat4 Voxel::Player::getViewMatrix()
{
	return glm::translate(viewMatrix, -position);
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

void Voxel::Player::update()
{
	moved = false;
	rotated = false;
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
		rayMat = glm::translate(rayMat, position);
		rayMat = glm::rotate(rayMat, glm::radians(-rotation.y), glm::vec3(0, 1, 0));
		rayMat = glm::rotate(rayMat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		rayMat = glm::rotate(rayMat, glm::radians(-rotation.z), glm::vec3(0, 0, 1));

		lineProgram->setUniformMat4("modelMat", rayMat);
		glDrawArrays(GL_LINES, 0, 2);
	}
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
	
	// Angle x range: 0~90, 360 ~ 270
	wrapAngle(rotation.x);
	wrapAngleX();
	//std::cout << "x = " << rotation.x << std::endl;

	updateViewMatrix();
	updateDirMatrix();
	updateDirection();
	rotated = true;
}

void Voxel::Player::addRotationY(const float y)
{
	//std::cout << "y = " << y << std::endl;
	rotation.y += y * rotationSpeed;

	wrapAngle(rotation.y);
	updateViewMatrix();
	updateDirMatrix();
	updateDirection();
	rotated = true;
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
