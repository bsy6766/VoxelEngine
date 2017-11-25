#include "Player.h"
#include <Camera.h>
#include <Utility.h>
#include <ProgramManager.h>
#include <Application.h>
#include <Program.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/compatibility.hpp>

using namespace Voxel;

const float Player::MaxCameraDistanceX = 10.0f;
const float Player::EyeHeight = 1.5f;

Player::Player()
	: position(0)
	, nextPosition(0)
	, rotation(0)
	, rotationTarget(0)
	, movementSpeed(7.0f)
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
	, onGround(false)
	, cameraY(Player::EyeHeight)
	, cameraDistanceZ(Player::MaxCameraDistanceX)
	, cameraDistanceTargetZ(Player::MaxCameraDistanceX)
	, viewMode(ViewMode::FIRST_PERSON_VIEW)
	, jumpState(JumpState::FALLING)
	, cameraColliding(false)
	, xzVelocity(0)
	, xzAcceleration(glm::vec2(0.05f, 0.05f))
	, xzElapsedTime(0)
#if V_DEBUG
#if V_DEBUG_PLAYER_DIR_LINE
	, yLineVao(0)
	, rayVao(0)
#endif
#if V_DEBUG_PLAYER_BOUNDING_BOX
	, boundingBoxVao(0)
#endif
#endif
{

}

Player::~Player()
{
#if V_DEBUG
#if V_DEBUG_PLAYER_DIR_LINE
	if (yLineVao)
	{
		glDeleteVertexArrays(1, &yLineVao);
	}

	if (rayVao)
	{
		glDeleteVertexArrays(1, &rayVao);
	}
#endif
#if V_DEBUG_PLAYER_BOUNDING_BOX
	if (boundingBoxVao)
	{
		glDeleteVertexArrays(1, &boundingBoxVao);
	}
#endif
#endif
}

void Voxel::Player::init()
{
	// Todo: load player info from save file
	this->direction = glm::vec3(0, 0, -1);
	// can reach up to 5 blocks from position
	this->rayRange = 5.0f;

#if V_DEBUG
#if V_DEBUG_PLAYER_DIR_LINE
	initYLine();
	initRayLine();
#endif
#if V_DEBUG_PLAYER_BOUNDING_BOX
	initBoundingBoxLine();
#endif
#endif
}

void Player::moveFoward(const float delta)
{
	nextPosition += getMovedDistByKeyInput(-180.0f, glm::vec3(0, 1, 0), movementSpeed * delta);
	moved = true;
	/*
	xzElapsedTime.x += delta;
	xzVelocity.x += (xzAcceleration.x * xzElapsedTime.x);

	if (xzVelocity.x > 1.0f)
	{
		xzVelocity.x = 1.0f;
	}

	nextPosition += getMovedDistByKeyInput(-180.0f, glm::vec3(0, 1, 0), xzVelocity.x);

	std::cout << "v = " << Utility::Log::vec2ToStr(xzVelocity) << std::endl;
	*/
}

void Voxel::Player::stopFoward(const float delta)
{
	/*
	if (xzElapsedTime.x != 0.0f)
	{
		xzElapsedTime.x = glm::lerp(xzElapsedTime.x, 0.0f, 10.0f * delta);

		if (glm::abs(xzElapsedTime.x) < 0.001f)
		{
			xzElapsedTime.x = 0.0f;
		}
	}

	if(xzVelocity.x )
	*/
}

void Player::moveBackward(const float delta)
{
	nextPosition += getMovedDistByKeyInput(0, glm::vec3(0, 1, 0), movementSpeed * delta);
	moved = true;
	/*
	xzElapsedTime.x -= delta;
	xzVelocity.x -= (xzAcceleration.x * xzElapsedTime.x);

	if (xzVelocity.x < -1.0f)
	{
		xzVelocity.x = -1.0f;
	}

	nextPosition += getMovedDistByKeyInput(0.0f, glm::vec3(0, 1, 0), xzVelocity.x);

	std::cout << "v = " << Utility::Log::vec2ToStr(xzVelocity) << std::endl;
	*/
}

void Voxel::Player::stopBackward(const float delta)
{
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

void Player::jump()
{
	onGround = false;

	jumpState = JumpState::JUMPING;
}

void Player::sneak()
{

}

void Voxel::Player::setFly(const bool mode)
{
	fly = mode;

	if (fly)
	{
		setOnGround(true);
		setJumpState(Voxel::Player::JumpState::IDLE);
	}
	else
	{
		setOnGround(false);
	}
}

glm::mat4 Voxel::Player::getViewMatrix()
{
	if (viewMode == ViewMode::FIRST_PERSON_VIEW)
	{
		// Player's view point is First persone view where it starts from the eye position
		//return glm::translate(viewMatrix, -position) * glm::translate(glm::mat4(1.0f), glm::vec3(0, -Player::EyeHeight, 0));
		return viewMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0, -Player::EyeHeight, 0));
	}
	else
	{
		// Third person view.
		// 1) Rotate and traslate in thrid person view. Use cameraDistanceZ to move back (cam pos) and rotate using player's rotation.
		// 2) Translate new matrix to player's position.
		// 3) mutliple (1) and (2). 
		// note: Move world in front of camera. From there, rotate the world. projection * View&Model(rot * trans) -> Trans goes first and rotates, then projected to screen.
		//return glm::rotate(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, 0, -cameraDistanceZ)), glm::radians(-rotation.x), glm::vec3(1, 0, 0)), glm::radians(rotation.y), glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1), -(glm::vec3(position.x, position.y + cameraY, position.z)));
		return glm::rotate(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, 0, -cameraDistanceZ)), glm::radians(-rotation.x), glm::vec3(1, 0, 0)), glm::radians(rotation.y), glm::vec3(0, 1, 0)) * getCameraYMatrix();
	}
}

glm::mat4 Voxel::Player::getWorldMatrix() const
{
	return glm::translate(glm::mat4(1.0f), -(glm::vec3(position.x, position.y, position.z)));
}

glm::mat4 Voxel::Player::getCameraYMatrix() const
{
	return glm::translate(glm::mat4(1.0f), -(glm::vec3(0.0f, cameraY, 0.0f)));
}

glm::mat4 Voxel::Player::getPosMatrix() const
{
	return glm::mat4();
}

glm::mat4 Voxel::Player::getFrustumViewMatrix()
{
	// In this case, inverse rotation
	//return glm::translate(glm::mat4(1.0f), position) * glm::translate(glm::mat4(1.0f), glm::vec3(0, -Player::EyeHeight, 0)) * glm::inverse(viewMatrix);
	return glm::translate(glm::mat4(1.0f), glm::vec3(0, Player::EyeHeight, 0)) * glm::inverse(viewMatrix);
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

glm::mat4 Voxel::Player::getOrientation()
{
	return viewMatrix;
}
*/

glm::mat4 Voxel::Player::getTranslationMat()
{
	return glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 Voxel::Player::getTranslationXZMat()
{
	return glm::translate(glm::mat4(1.0f), glm::vec3(position.x, 0, position.z));
}

glm::mat4 Voxel::Player::getSkyboxMat(const bool addPosition) const
{
	if (addPosition)
	{
		return glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(position.x, 70.0f, position.z)), glm::radians(-rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
	{
		return glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 70.0f, 0.0f)), glm::radians(-rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	}
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

float Voxel::Player::getCameraDistanceZ()
{
	return cameraDistanceTargetZ;
}

float Voxel::Player::getMaxCameraDistanceZ()
{
	return Voxel::Player::MaxCameraDistanceX;
}

float Voxel::Player::getCameraY()
{
	return cameraY;
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

	// take fall damage

	// reset values if player is on ground
	if (onGround)
	{
		// run cooldown;
		//jumpCooldown = 0;
		//jumpDistance = Player::DefaultJumpDistance;
		//jumpState = JumpState::IDLE;
		//std::cout << "Player is on ground\n";

		fallDistance = 0.0f;
		fallDuration = 0.0f;
	}
	else
	{

	}
}

bool Voxel::Player::isFlying()
{
	return fly;
}

glm::vec3 Voxel::Player::getDirection()
{
	return direction;
}

glm::vec3 Voxel::Player::getNextDirection()
{
	auto nextDirMatrix = mat4(1.0f);
	nextDirMatrix = glm::rotate(nextDirMatrix, glm::radians(-rotationTarget.y), glm::vec3(0, 1, 0));
	nextDirMatrix = glm::rotate(nextDirMatrix, glm::radians(rotationTarget.x), glm::vec3(1, 0, 0));
	nextDirMatrix = glm::rotate(nextDirMatrix, glm::radians(-rotationTarget.z), glm::vec3(0, 0, 1));

	return vec3(nextDirMatrix * vec4(0, 0, -1, 1));
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
	this->cameraDistanceTargetZ -= 1.0f;
	if (this->cameraDistanceTargetZ < 1.0f)
	{
		this->cameraDistanceTargetZ = 1.0f;
	}
}

void Voxel::Player::zoomOutCamera()
{
	this->cameraDistanceTargetZ += 1.0f;
	if (this->cameraDistanceTargetZ > Player::MaxCameraDistanceX)
	{
		this->cameraDistanceTargetZ = Player::MaxCameraDistanceX;
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

Shape::AABB Voxel::Player::getBoundingBox()
{
	return Shape::AABB(glm::vec3(position.x, position.y + 0.75f, position.z), glm::vec3(0.8f, 1.5f, 0.8f));
}

Shape::AABB Voxel::Player::getBoundingBox(const glm::vec3 & position)
{
	return Shape::AABB(glm::vec3(position.x, position.y + 0.75f, position.z), glm::vec3(0.8f, 1.5f, 0.8f));
	//return Shape::AABB(glm::vec3(position.x, position.y + 0.75f, position.z), glm::vec3(1.0f, 1.5f, 1.0f));
}

void Voxel::Player::autoJump(const float y)
{
	position.y += y;
	nextPosition.y += y;
	cameraY -= y;

	// Not sure why I added. this. Removing this solves weird stutter looking after auto jump
	//position.x = nextPosition.x;
	//position.z = nextPosition.z;

	moved = true;
}

void Voxel::Player::autoJump(const glm::vec3 & resolvedPosition)
{
	position = resolvedPosition;
	nextPosition = resolvedPosition;
	cameraY -= resolvedPosition.y;

	moved = true;
}

bool Voxel::Player::isJumping()
{
	return jumpState == JumpState::JUMPING;
}

bool Voxel::Player::isFalling()
{
	return jumpState == JumpState::FALLING;
}

bool Voxel::Player::isJumpStateIdle()
{
	return jumpState == JumpState::IDLE;
}

void Voxel::Player::setJumpState(const Voxel::Player::JumpState jumpState)
{
	this->jumpState = jumpState;
}

Voxel::Player::JumpState Voxel::Player::getJumpState() const
{
	return this->jumpState;
}

void Voxel::Player::setResolvedCameraDistanceZ(const float dist)
{
	if (dist < cameraDistanceTargetZ)
	{
		auto adjust = dist;// -0.75f;
		if (adjust < 0.0f)
		{
			adjust = 0.0f;
		}
		cameraDistanceZ = adjust;
	}
}

void Voxel::Player::setCameraColliding(const bool value)
{
	cameraColliding = value;
}

bool Voxel::Player::isOnFPViewMode()
{
	return this->viewMode == ViewMode::FIRST_PERSON_VIEW;
}

bool Voxel::Player::isOnTPViewMode()
{
	return this->viewMode == ViewMode::THIRD_PERSON_VIEW;
}

glm::vec3 Voxel::Player::getMovedDistByKeyInput(const float angleMod, const glm::vec3& axis, float distance)
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
	//viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), vec3(0, 0, 1));
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
	//std::cout << "Updating player view direction (" << direction.x << ", " << direction.y << ", " << direction.z << ")\n";
}

void Voxel::Player::update(const float delta)
{
	//std::cout << "js = " << std::to_string((int)jumpState) << "\n";

	rotated = false;

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

		rotated = true;
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
		rotated = true;
	}

	if (rotated)
	{
		updateViewMatrix();
		updateDirMatrix();
		updateDirection();
	}

	if (cameraY != Player::EyeHeight)
	{
		cameraY = glm::lerp(cameraY, Player::EyeHeight, 6.0f * delta);
		if (glm::abs(cameraY - Player::EyeHeight) < 0.0001f)
		{
			cameraY = Player::EyeHeight;
		}

		//std::cout << "cy = " << cameraY << "\n";
	}
}

void Voxel::Player::updateMovement(const float delta)
{
	// Updates player movement. Call this after resolving collision.
	
	if (position.x != nextPosition.x)
	{
		position.x = glm::lerp(position.x, nextPosition.x, 20.0f * delta);

		if (glm::abs(position.x - nextPosition.x) <= 0.01f)
		{
			position.x = nextPosition.x;
		}
	}

	if (position.z != nextPosition.z)
	{
		position.z = glm::lerp(position.z, nextPosition.z, 20.0f * delta);

		if (glm::abs(position.z - nextPosition.z) <= 0.01f)
		{
			position.z = nextPosition.z;
		}
	}

	if (position.y != nextPosition.y)
	{
		// Current position and next position is different. lerp to next position.

		// First save current Y for cameraY.
		const float curY = position.y;

		// Lerp it
		position.y = glm::lerp(position.y, nextPosition.y, 20.0f * delta);
		
		if (glm::abs(position.y - nextPosition.y) <= 0.05f)
		{
			// Check distance between. If it's less than 0.001f, just use next position.
			position.y = nextPosition.y;
		}
		
		// Add y offset to cameraY to give smooth camera following effect.
		addCameraY((curY - position.y) * 0.5f);
	}
}

void Voxel::Player::updateCameraDistanceZ(const float delta)
{
	if (cameraColliding) return;

	if (cameraDistanceZ != cameraDistanceTargetZ)
	{
		cameraDistanceZ = glm::lerp(cameraDistanceZ, cameraDistanceTargetZ, 2.0f * delta);

		if (glm::abs(cameraDistanceTargetZ - cameraDistanceZ) < 0.001f)
		{
			cameraDistanceZ = cameraDistanceTargetZ;
		}
	}
	//std::cout << "cameraDistanceZ = " << cameraDistanceZ << "\n";
	//std::cout << "cameraDistanceTargetZ = " << cameraDistanceTargetZ << "\n";
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

void Voxel::Player::setResolvedNextPosition(const glm::vec3 & resolvedNextPosition)
{
	/*
	if (moved)
	{
	float y = glm::abs(resolvedNextPosition.y - position.y);
	cameraY += y;
	}
	*/
	this->nextPosition = resolvedNextPosition;
}

void Voxel::Player::addCameraY(const float y)
{
	//std::cout << "adding cy  = " << y << "\n";
	cameraY += y;
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

glm::vec3 Voxel::Player::getNextEyePosition()
{
	return glm::vec3(nextPosition.x, nextPosition.y + Player::EyeHeight, nextPosition.z);
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

void Voxel::Player::setRotation(const glm::vec3 & newRotation, const bool smoothRotation)
{
	if (smoothRotation)
	{
		rotationTarget = newRotation;
	}
	else
	{
		rotation = newRotation;
		rotationTarget = newRotation;

		wrapAngle();

		updateViewMatrix();
		updateDirMatrix();
		updateDirection();
		rotated = true;
	}
}

glm::vec3 Voxel::Player::getRotation()
{
	return rotation;
}

#if V_DEBUG
void Voxel::Player::renderDebugLines(Program* lineProgram)
{
	int vertSize = 0;
	int drawCall = 0;

#if V_DEBUG_PLAYER_DIR_LINE
	if (yLineVao)
	{
		glBindVertexArray(yLineVao);

		lineProgram->setUniformMat4("modelMat", glm::mat4(1.0f));
		glDrawArrays(GL_LINES, 0, 2);

		drawCall++;
		vertSize++;
	}

	if (rayVao)
	{
		glBindVertexArray(rayVao);

		glm::mat4 rayMat = mat4(1.0f);
		rayMat = glm::translate(rayMat, glm::vec3(0.0f, Player::EyeHeight, 0.0f));
		rayMat = glm::rotate(rayMat, glm::radians(-rotation.y), glm::vec3(0, 1, 0));
		rayMat = glm::rotate(rayMat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		rayMat = glm::rotate(rayMat, glm::radians(-rotation.z), glm::vec3(0, 0, 1));

		lineProgram->setUniformMat4("modelMat", rayMat);
		glDrawArrays(GL_LINES, 0, 2);

		drawCall++;
		vertSize++;
	}
#endif

#if V_DEBUG_PLAYER_BOUNDING_BOX
	if (boundingBoxVao)
	{
		glBindVertexArray(boundingBoxVao);
		lineProgram->setUniformMat4("modelMat", glm::mat4(1.0f));
		glDrawArrays(GL_LINES, 0, 24);

		drawCall++;
		vertSize += 12;
	}
#endif

	auto glView = Application::getInstance().getGLView();
#if V_DEBUG_COUNT_DRAW_CALLS
	if (glView->doesCountDrawCalls())
	{
		for(int i = 0; i < drawCall; i++)
			glView->incrementDrawCall();
	}
#endif
#if V_DEBUG_COUNT_VISIBLE_VERTICES
	if (glView->doesCountVerticesSize())
	{
		glView->addVerticesSize(vertSize);
	}
#endif
}

#if V_DEBUG_PLAYER_DIR_LINE
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
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
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
	auto program = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
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
#endif

#if V_DEBUG_PLAYER_BOUNDING_BOX
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
	auto program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::LINE_SHADER);
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
}
#endif
#endif