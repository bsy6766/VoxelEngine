#include "Application.h"

// cpp
#include <iostream>
#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>

// voxel
#include "Director.h"
#include "Camera.h"
#include "InputHandler.h"
#include "DataTree.h"
#include "Setting.h"
#include "FileSystem.h"
#include "Utility.h"
#include "MenuScene.h"

using std::cout;
using std::endl;
using namespace Voxel;

Application::Application()
	: glView(nullptr)
	, director(nullptr)
	, internalSetting(nullptr)
	, needToSkipFrame(true)
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

	// Read game settings
	Setting::getInstance();
}

Application::~Application()
{
	cout << "Destroying Application" << endl; 
	// Everything should be deleted first in terminate();
}

void Application::init()
{
	initInternalSettings();

	initGLView();

	initMainCamera();

	initDirector();
}

void Voxel::Application::initGLView()
{
	glView = new GLView();

	//auto title = "Voxel Engine / version: 0 / build: " + internalSetting->getString("buildNumber");
	auto title = "Voxel Engine";

	auto& setting = Setting::getInstance();
	auto resolution = setting.getResolution();

	glView->init(resolution.x, resolution.y, title, setting.getWindowMode(), setting.getVsync());
}

void Voxel::Application::initMainCamera()
{
	auto& setting = Setting::getInstance();
	auto resolution = glm::vec2(setting.getResolution());
	auto fov = setting.getFieldOfView();
	float near = internalSetting->getFloat("Camera.near");
	float far = internalSetting->getFloat("Camera.far");

	Camera::mainCamera = Camera::create(glm::vec3(0, 0, 0), static_cast<float>(fov), near, far, resolution.x, resolution.y);
}

void Voxel::Application::initDirector()
{
	//game = new Game();
	//game->init();
	//game->createNew("New World");
	director = new Director();
	director->runScene(Voxel::Director::SceneName::MENU_SCENE);
}

void Voxel::Application::initInternalSettings()
{
	if (internalSetting) 
	{
		delete internalSetting;
	}

	internalSetting = DataTree::create("data/internal");

	if (internalSetting)
	{
		int build = internalSetting->getInt("buildNumber");
		build++;
		internalSetting->setInt("buildNumber", build);
		internalSetting->save("data/internal");
	}
	else
	{
		std::cout << "[Application] Failed to load internal setting\n";
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
		glView->clearBufferBit();

		glView->updateTime();
		glView->updateFPS();
		
		float delta = static_cast<float>(glView->getElaspedTime());
		
		if (needToSkipFrame)
		{
			needToSkipFrame = false;
		}
		else
		{
			input.update();

			director->update(delta);
		}

		// Wipe input data for current frame
		input.postUpdate();

		director->render();

		glView->render();
	}

	cleanUp();
}

void Voxel::Application::end()
{
	glView->close();
}

void Voxel::Application::skipFrame()
{
	needToSkipFrame = true;
}

GLView * Voxel::Application::getGLView()
{
	return glView;
}

Director * Voxel::Application::getDirector()
{
	return director;
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

	if (director)
	{
		delete director;
	}

	if (glView)
	{
		delete glView;
	}

	if (internalSetting)
	{
		delete internalSetting;
	}
}