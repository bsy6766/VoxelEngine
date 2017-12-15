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
#include "Utility.h"
#include "MenuScene.h"
#include "Cursor.h"
#include "FontManager.h"
#include "FileSystem.h"
#include "Logger.h"
#include "GLView.h"

// boost. 
//#include <boost\predef.h>

using std::cout;
using std::endl;
using namespace Voxel;

Application::Application()
	: glView(nullptr)
	, director(nullptr)
	, needToSkipFrame(true)
{
	// init file system
	auto fs = &Voxel::FileSystem::getInstance();

	// init logger
	auto logger = &Voxel::Logger::getInstance();

	logger->info("[Application] Initializing application");

	/*
	// old version
	cout << "Creating Application" << endl;

	char cCurrentPath[FILENAME_MAX];

	if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
	{
		workingDirectory = "";
	}
	else
	{
		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; // not really required

		this->workingDirectory = std::string(cCurrentPath);
	}
	*/
}

Application::~Application()
{
	cleanUp();

	Voxel::Logger::getInstance().flush();
}

void Application::init()
{	
#if V_BUILD_NUMBER
	initInternalSettings();
#endif

	// initialize glview
	initGLView();

	initMainCamera();

	initFonts();

	initSpriteSheets();

	initDirector();

	// init cursor
	cursor	= &Cursor::getInstance();
	cursor->init();

	Voxel::Logger::getInstance().flush();
}

void Voxel::Application::initGLView()
{
	glView = new GLView();

	//auto title = "Voxel Engine / version: 0 / build: " + internalSetting->getString("buildNumber");
	
	glView->init("Voxel Engine");
}

void Voxel::Application::initMainCamera()
{
	auto& setting = Setting::getInstance();
	auto resolution = glm::vec2(setting.getResolution());
	auto fov = setting.getFieldOfView();
	float nears = 0.01f;
	float fars = 1000.0f;

	// Create main camera
	Camera::mainCamera = Camera::create(glm::vec3(0, 0, 0), static_cast<float>(fov), nears, fars, resolution.x, resolution.y);
}

void Voxel::Application::initDirector()
{
	director = new Director();
	director->runScene(Voxel::Director::SceneName::MENU_SCENE);
}

void Voxel::Application::initFonts()
{
	auto& fm = FontManager::getInstance();

	fm.addFont("Pixel.ttf", 10);
	fm.addFont("Pixel.ttf", 10, 2);
}

void Voxel::Application::initSpriteSheets()
{
	auto& sm = SpriteSheetManager::getInstance();

	sm.addSpriteSheet("GlobalSpriteSheet.json");
	sm.addSpriteSheet("CursorSpriteSheet.json");

#if V_DEBUG
	sm.addSpriteSheet("DebugSpriteSheet.json");
#endif
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

		// render fade
		director->renderFade();

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
		director = nullptr;
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
		glView = nullptr;
	}
}

#if V_BUILD_NUMBER
void Voxel::Application::initInternalSettings()
{
	auto internalSetting = DataTree::create(Voxel::FileSystem::getInstance().getWorkingDirectory() + "/../buildNumber");

	if (internalSetting)
	{
		int build = internalSetting->getInt("buildNumber");
		build++;
		internalSetting->setInt("buildNumber", build);
		internalSetting->save(Voxel::FileSystem::getInstance().getWorkingDirectory() + "/../buildNumber");
	}
}
#endif