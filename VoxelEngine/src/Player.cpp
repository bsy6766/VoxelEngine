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
	// Debug
	, yLineVao(0)
	, yLineVbo(0)
	, rayVao(0)
	, rayVbo(0)
{

}

Player::~Player()
{

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
	auto defaultProgram = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);
	GLint vertLoc = defaultProgram->getAttribLocation("vert");
	GLint colorLoc = defaultProgram->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
}

void Voxel::Player::initRayLine()
{
	auto playerRayEnd = getRayEnd();
	// Generate vertex array object
	glGenVertexArrays(1, &rayVao);
	// Bind it
	glBindVertexArray(rayVao);

	// Generate buffer object
	glGenBuffers(1, &rayVbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, rayVbo);

	GLfloat ray[] = {
		position.x, position.y, position.z, 1, 0, 0,
		playerRayEnd.x, playerRayEnd.y, playerRayEnd.z, 1, 0, 0
	};

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(ray), ray, GL_STATIC_DRAW);
	// Enable vertices attrib
	auto defaultProgram = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);
	GLint vertLoc = defaultProgram->getAttribLocation("vert");
	GLint colorLoc = defaultProgram->getAttribLocation("color");
	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
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

glm::mat4 Voxel::Player::getDirVP(const glm::mat4 & projection)
{
	return glm::translate(projection * dirMatrix, -position);
}

glm::mat4 Voxel::Player::getOrientation()
{
	return viewMatrix;
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

void Voxel::Player::render(Program* defaultProgram)
{
	if (yLineVao)
	{
		glBindVertexArray(yLineVao);

		glm::mat4 lineMat = mat4(1.0f);
		lineMat = glm::translate(lineMat, position);
		//lineMat = glm::rotate(lineMat, glm::radians(-rotation.y), glm::vec3(0, 1, 0));
		//lineMat = glm::rotate(lineMat, glm::radians(-rotation.x), glm::vec3(1, 0, 0));
		//lineMat = glm::rotate(lineMat, glm::radians(-rotation.z), glm::vec3(0, 0, 1));

		//defaultProgram->setUniformMat4("cameraMat", getVP(Camera::mainCamera->getProjection()));
		defaultProgram->setUniformMat4("modelMat", lineMat);
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

		defaultProgram->setUniformMat4("modelMat", rayMat);
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

void Voxel::Player::setRotation(const glm::vec3 & newRotation)
{
	rotation = newRotation;
	updateViewMatrix();
	updateDirMatrix();
	updateDirection();
	rotated = true;
}

glm::vec3 Voxel::Player::getRotation()
{
	return rotation; 
}
