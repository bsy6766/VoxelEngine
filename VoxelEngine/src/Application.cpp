#include "Application.h"

#include <iostream>

#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>

#include <Utility.h>

#include <World.h>

#include <Camera.h>
#include <InputHandler.h>

using std::cout;
using std::endl;
using namespace Voxel;

Application::Application()
	: world(nullptr)
	, glView(nullptr)
{
	cout << "Creating Application" << endl;

	char cCurrentPath[FILENAME_MAX];

	if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
	{
		workingDirectory = "";
	}
	else
	{
		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

		this->workingDirectory = std::string(cCurrentPath);
	}

	cout << "Working Directory is: " << workingDirectory << endl;
}

Application::~Application()
{
	cout << "Destroying Application" << endl; 
	// Everything should be deleted first in terminate();
}

void Application::init()
{
	Utility::Random::setSeed("ENGINE");

	initGLView();

	initMainCamera();
	initWorld();
}

void Voxel::Application::initGLView()
{
	glView = new GLView();
	glView->init();
}

void Voxel::Application::initMainCamera()
{
	//Camera::mainCamera = Camera::create(vec3(0), 70.0f, 0.03f, 200.0f, 1280.0f / 720.0f);
	//Camera::mainCamera->addAngle(glm::vec3(0));
	Camera::mainCamera = Camera::create(vec3(0, 0, 0), 70.0f, 0.05f, 500.0f, 1280.0f / 720.0f);
	Camera::mainCamera->addAngle(glm::vec3(0, 180, 0));
}

void Voxel::Application::initWorld()
{
	world = new World();
}

void Application::run()
{
	auto& input = InputHandler::getInstance();
	input.setCursorToCenter();
	input.initControllerManager();
	input.update();

	glView->resetTime();
	
	while (glView->isRunning())
	{
		glView->clearBuffer();

		glView->updateTime();
		glView->updateFPS();
		
		float delta = static_cast<float>(glView->getElaspedTime());

		input.update();

		world->update(delta);

		world->render(delta);

		glView->render();
	}

	cleanUp();
}

void Voxel::Application::end()
{
	glView->close();
}

GLView * Voxel::Application::getGLView()
{
	return glView;
}

void Voxel::Application::cleanUp()
{
	// delete everything here before GLFW window dies

	// Delete main Camera
	if (Camera::mainCamera)
	{
		delete Camera::mainCamera;
		Camera::mainCamera = nullptr;
	}

	if (world)
	{
		world->threadRunning = false;
		world->cv.notify_all();
		delete world;
	}

	if (glView)
	{
		delete glView;
	}
}