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
#include "Cursor.h"
#include "FontManager.h"

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
	std::cout << "[Application] Initializing application\n";
	initInternalSettings();

	initGLView();

	initMainCamera();

	auto& fm = FontManager::getInstance();

	fm.addFont("Pixel.ttf", 10);
	fm.addFont("Pixel.ttf", 10, 2);

	initDirector();

	// init cursor
	cursor	= &Cursor::getInstance();
	cursor->init();
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
	// get input handler
	auto& input = InputHandler::getInstance();
	// reset mouse position
	input.setCursorToCenter();
	// init controller manager
	input.initControllerManager();
	// update. This updates controller manager and detects controllers
	input.update();

	// Reset time
	glView->resetTime();
	
	// Iterate while GLView is running
	while (glView->isRunning())
	{
		// Clear screen and depth test
		glView->clearRender();

		// Update time and fps
		glView->updateTime();
		glView->updateFPS();
		
		// Check if need to skip frame
		if (needToSkipFrame)
		{
			// Skip. Doesn't update game.
			needToSkipFrame = false;
		}
		else
		{
			// Not skipping frame. Update game.
			
			// Update input handler
			// Note: this only updates controllers.
			input.update();

			// Update cursor pos
			if (cursor->isVisible())
			{
				cursor->addPosition(input.getMouseMovedDistance());
				//cursor->setPosition(input.getMousePosition());
			}

			// Update director
			director->update(static_cast<float>(glView->getElaspedTime()));
		}

		// Wipe input data for current frame
		input.postUpdate();

		// Render director
		director->render();

		// Clear depth buffer and render above current buffer
		glClear(GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_ALWAYS);

		// render cursor
		cursor->render();

		// Swap buffer and poll events. All glfw events are called here.
		glView->render();

		// loop finished
	}
	// Main while loop ends
	
	// Clean up everything.
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

	// Release cursor
	Cursor::getInstance().release();

	// Release fonts
	FontManager::getInstance().clear();

	// Release director
	if (director)
	{
		// This release current scene and next scene.
		// These scenes should release their own sprite sheets.
		delete director;
	}

	// Release spritesheet. SpriteSheets that are used for each scene should be relased at this point. Release remaining.
	SpriteSheetManager::getInstance().releaseAll();

	// Release texture. SpriteSheet releases texture too. Release remainig.
	TextureManager::getInstance().releaseAll();

	// debug print
	SpriteSheetManager::getInstance().print(true);
	TextureManager::getInstance().print();

	if (glView)
	{
		delete glView;
	}

	if (internalSetting)
	{
		delete internalSetting;
	}
}