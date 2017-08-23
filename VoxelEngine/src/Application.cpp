#include "Application.h"

#include <iostream>

#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>

#include <Utility.h>

#include <World.h>

#include <Camera.h>
#include <InputHandler.h>

#include <DataTree.h>
#include <FileSystem.h>

using std::cout;
using std::endl;
using namespace Voxel;

Application::Application()
	: world(nullptr)
	, glView(nullptr)
	, configData(nullptr)
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

	// initialize singleton instance
	FileSystem::getInstance();
}

Application::~Application()
{
	cout << "Destroying Application" << endl; 
	// Everything should be deleted first in terminate();
}

void Application::init()
{
	Utility::Random::setSeed("ENGINE");

	initConfig();

	initGLView();

	initMainCamera();
	initWorld();
}

void Voxel::Application::initGLView()
{
	glView = new GLView();

	auto title = configData->getString("system.window.title") + " / version: 0 / build: " + configData->getString("build.number");

	glView->init(configData->getInt("system.window.resolution.width"),
		configData->getInt("system.window.resolution.height"), 
		title,
		configData->getInt("system.window.mode"),
		configData->getBool("system.window.vsync"));
}

void Voxel::Application::initMainCamera()
{
	auto sWidth = configData->getFloat("system.window.resolution.width");
	auto sHeight = configData->getFloat("system.window.resolution.height");
	Camera::mainCamera = Camera::create(vec3(0, 0, 0), 70.0f, 0.05f, 1000.0f, sWidth, sHeight);
	//Camera::mainCamera->addAngle(glm::vec3(0, 180, 0));
}

void Voxel::Application::initWorld()
{
	world = new World();
	world->createNew("New World");
}

void Voxel::Application::initConfig()
{
	if (configData) 
	{
		delete configData;
	}

	configData = DataTree::create("config/config");

	if (configData)
	{
		int build = configData->getInt("build.number");
		build++;
		configData->setInt("build.number", build);
		configData->save("config/config");
	}
	else
	{
		std::cout << "[Application] Failed to load config" << std::endl;
	}
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

std::string Voxel::Application::getWorkingDirectory()
{
	return workingDirectory;
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
		delete world;
	}

	if (glView)
	{
		delete glView;
	}

	if (configData)
	{
		delete configData;
	}
}