#include "World.h"

#include <Chunk.h>
#include <InputHandler.h>
#include <Camera.h>

#include <GLFW\glfw3.h>

using namespace Voxel;

World::World()
{
	cameraMovementSpeed = 30.0f;
	testChunk = Chunk::create(0, 0);
	input = &InputHandler::getInstance();

	double x, y;
	input->getMousePosition(x, y);
	prevX = static_cast<float>(x);
	prevY = static_cast<float>(y);
}

World::~World()
{
	// delete everything
	delete testChunk;
}

void World::update(const float delta)
{
	if (input->getKeyDown(GLFW_KEY_W))
	{
		Camera::mainCamera->addPosition(glm::vec3(0, 0, cameraMovementSpeed * delta));
	}
	else if (input->getKeyDown(GLFW_KEY_S))
	{
		Camera::mainCamera->addPosition(glm::vec3(0, 0, -cameraMovementSpeed * delta));
	}

	if (input->getKeyDown(GLFW_KEY_A))
	{
		Camera::mainCamera->addPosition(glm::vec3(cameraMovementSpeed * delta, 0, 0));
	}
	else if (input->getKeyDown(GLFW_KEY_D))
	{
		Camera::mainCamera->addPosition(glm::vec3(-cameraMovementSpeed * delta, 0, 0));
	}

	if (input->getKeyDown(GLFW_KEY_SPACE))
	{
		Camera::mainCamera->addPosition(glm::vec3(0, cameraMovementSpeed * delta, 0));
	}
	else if (input->getKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		Camera::mainCamera->addPosition(glm::vec3(0, -cameraMovementSpeed * delta, 0));
	}

	if (input->getKeyDown(GLFW_KEY_Q))
	{
		Camera::mainCamera->addAngle(glm::vec3(0, 15.0f * delta, 0));
	}
	else if (input->getKeyDown(GLFW_KEY_E))
	{
		Camera::mainCamera->addAngle(glm::vec3(0, -15.0f * delta, 0));
	}

	if (input->getKeyDown(GLFW_KEY_C))
	{
		Camera::mainCamera->setAngle(glm::vec3(0));
		Camera::mainCamera->setPosition(glm::vec3(0, 0, -100.0f));
	}

	if (input->getKeyDown(GLFW_KEY_V))
	{
		Camera::mainCamera->setAngle(glm::vec3(0, 180.0f, 0));
		Camera::mainCamera->setPosition(glm::vec3(0, 0, 100.0f));
	}

	/*
	double x, y;
	input->getMousePosition(x, y);

	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);

	float dx = xf - prevX;
	float dy = yf - prevY;

	Camera::mainCamera->addAngle(vec3(0, dx * delta * 30.0f, 0));
	Camera::mainCamera->addAngle(vec3(dy * delta * 30.0f, 0, 0));

	prevX = xf;
	prevY = yf;
	*/
}