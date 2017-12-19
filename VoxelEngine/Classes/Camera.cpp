// pch
#include "PreCompiled.h"

#include "Camera.h"

// voxel
#include "Frustum.h"
#include "ChunkUtil.h"
#include "ProgramManager.h"
#include "Program.h"
#include "Setting.h"
#include "Logger.h"

using namespace Voxel;
using namespace glm;

Camera* Camera::mainCamera = nullptr;
const float Camera::UIFovy = 70.0f;

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
	, frustum(nullptr)
	, screenSpacePos(0)
	, speed(15.0f)
{}

bool Voxel::Camera::init(const glm::vec3 & position, const float fovy, const float nears, const float fars, const float screenWidth, const float screenHeight)
{
	this->position = position;
	this->nears = nears;
	this->fars = fars;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->aspect = screenWidth / screenHeight;

	frustum = new Frustum();
	frustum->updateProjection(fovy, aspect, nears, fars);

	setFovy(fovy);

	// Refernce: http://wiki.panotools.org/Field_of_View
	this->fovx = glm::degrees(2.0f * atan(tan(glm::radians(fovy * 0.5f)) * this->aspect));

#if V_DEBUG && V_DEBUG_FRUSTUM_LINE
	frustum->initDebugLines(fovy, fovx, nears, static_cast<float>(Setting::getInstance().getRenderDistance()) * Constant::CHUNK_BORDER_SIZE);
#endif

	screenSpacePos = glm::vec3(0, 0, (screenHeight * 0.5f) / tan(glm::radians(fovy * 0.5f)));

#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	auto logger = &Voxel::Logger::getInstance();

	logger->consoleInfo("[Camera] Creating camera. Fovy: " + std::to_string(fovy) + ", Fovx: " + std::to_string(fovx) + ", Nears: " + std::to_string(nears) + ", fars: " + std::to_string(fars) + ", aspect: " + std::to_string(aspect));
	logger->consoleInfo("[Camera] Camera position (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z));
	logger->consoleInfo("[Camera] Camera rotation (" + std::to_string(angle.x) + ", " + std::to_string(angle.y) + ", " + std::to_string(angle.z));
	logger->consoleInfo("[Camera] Camera screen space (" + std::to_string(screenSpacePos.x) + ", " + std::to_string(screenSpacePos.y) + ", " + std::to_string(screenSpacePos.z));
#endif
	return true;
}

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

	if (newCamera->init(position, fovy, nears, fars, screenWidth, screenHeight))
	{
		return newCamera;
	}
	else
	{
		delete newCamera;
		return nullptr;
	}
}

Frustum * Voxel::Camera::getFrustum()
{
	return frustum;
}

void Voxel::Camera::updateScreenSizeAndAspect(const float screenWidth, const float screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	aspect = screenWidth / screenHeight;
	screenSpacePos = glm::vec3(0, 0, (screenHeight * 0.5f) / tan(glm::radians(fovy * 0.5f)));
}

void Voxel::Camera::updateProjection()
{
	ProgramManager::getInstance().updateProjMat(getProjection());
	ProgramManager::getInstance().updateUIProjMat(getProjection(Camera::UIFovy));
	
	frustum->updateProjection(fovy, aspect, nears, fars);
}

void Voxel::Camera::setFovy(const float fovy)
{
	this->fovy = fovy;

	updateProjection();
}

float Voxel::Camera::getFovy()
{
	return fovy;
}

glm::mat4 Voxel::Camera::getScreenSpaceMatrix()
{
	return glm::translate(glm::mat4(1.0f), -screenSpacePos);
}

mat4 Camera::getProjection()
{
	return perspective(glm::radians(fovy), aspect, nears, fars);
}

mat4 Voxel::Camera::getProjection(const float fovy)
{
	return perspective(glm::radians(fovy), aspect, nears, fars);
}

mat4 Camera::getViewMat()
{
	// View matrix defined the position (location and orientation) of the camera
	//return lookAt(position, vec3(0), vec3(0, 1, 0));
	return getOrientation();
}

glm::mat4 Voxel::Camera::getWorldMat()
{
	return glm::translate(glm::mat4(1.0f), -position);
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
}

void Camera::addPosition(const vec3 & distance)
{
	position += (distance * speed);
}

void Camera::setAngle(const vec3 & angle)
{
	this->angle = angle;
	wrapAngle();

	//std::cout << "[Camera] angle = (" << this->angle.x << ", " << this->angle.y << ", " << this->angle.z << ")\n";
}

void Camera::addAngle(const vec3 & angle)
{
	this->angle += angle;
	wrapAngle();

	//std::cout << "[Camera] angle = (" << this->angle.x << ", " << this->angle.y << ", " << this->angle.z << ")\n";
}

glm::vec3 Voxel::Camera::getAngle()
{
	return angle;
}

float Voxel::Camera::getAngleY()
{
	return angle.y;
}

float Voxel::Camera::getNear() const
{
	return nears;
}

void Voxel::Camera::setNear(const float nears)
{
	this->nears = nears;

	updateProjection();
}

float Voxel::Camera::getFar() const
{
	return fars;
}

void Voxel::Camera::setFar(const float fars)
{
	this->fars = fars;

	updateProjection();
}

void Voxel::Camera::setSpeed(const float speed)
{
	this->speed = speed;
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
	std::cout << "[Camera] position = (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")\n";
	std::cout << "[Camera] angle = (" << this->angle.x << ", " << this->angle.y << ", " << this->angle.z << ")\n";

	int chunkX = static_cast<int>(position.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(position.z) / Constant::CHUNK_SECTION_LENGTH;

	if (position.x < 0) chunkX -= 1;
	if (position.z < 0) chunkZ -= 1;

	std::cout << "[Camera] chunk pos = (" << chunkX << ", " << chunkZ << ")\n";
}