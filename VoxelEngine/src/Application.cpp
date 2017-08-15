#include "Application.h"

#include <iostream>

#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>

#include <InputHandler.h>
#include <ShaderManager.h>
#include <ProgramManager.h>

#include <World.h>

#include <Camera.h>

using std::cout;
using std::endl;
using namespace Voxel;

Application::Application()
	: world(nullptr)
	//, controllerManager(nullptr)
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
	
	if (window) 
	{
		glfwDestroyWindow(window);
		window = nullptr;
	}

	glfwTerminate();

	// Delete main Camera
	if (Camera::mainCamera)
	{
		delete Camera::mainCamera;
		Camera::mainCamera = nullptr;
	}
	
	// Delete all shaders
	ShaderManager::getInstance().releaseAll();
	ProgramManager::getInstance().releaseAll();

	if (world)
	{
		delete world;
	}
}

void Application::init()
{
	initGLFW();
	initWindow();
	initGLEW();
	initOpenGL();

	initMainCamera();
	initWorld();
	initTime();

	lastTime = 0;
}

void Application::initGLFW()
{
	if (glfwInit() != GL_TRUE)
	{
		// GLFW failed to initailize.
		throw std::runtime_error("Failed to initialize GLFW");
	}
	else
	{
		// Initialzied GLFW.
		int versionMajor, versionMinor, versionRev;
		glfwGetVersion(&versionMajor, &versionMinor, &versionRev);

		cout << "Intialized GLFW." << endl;
		cout << "GLFW version " << versionMajor << "." << versionMinor << "." << versionRev << endl;
	}
}

void Application::initWindow()
{
	//set to OpenGL 4.3
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//Set the color info
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);

	// TODO: make config for these
	bool fullscreen = false;
	bool borderelss = false;
	int screenWidth = 1280;
	int screenHeight = 720;
	std::string title = "Voxel Engine";

	glfwWindowHint(GLFW_AUTO_ICONIFY, fullscreen ? GL_TRUE : GL_FALSE);

	GLFWmonitor* monitor = nullptr;

	if (fullscreen)
	{
		// TODO: make window fullscreen
	}
	else
	{
		// Not fullscreen, but can be borderless
		glfwWindowHint(GLFW_DECORATED, borderelss ? GL_FALSE : GL_TRUE);
	}

	window = glfwCreateWindow(screenWidth, screenHeight, title.c_str(), monitor, nullptr);

	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("GLFW failed to create window");
	}

	// if window successfully made, make it current window
	glfwMakeContextCurrent(window);

	// Todo: move to config
	bool vsync = true;
	if (vsync)
	{
		glfwSwapInterval(0);
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set callbacks
	glfwSetErrorCallback(glfwErrorCallback);
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, InputHandler::glfwKeyCallback);
	glfwSetCursorPosCallback(window, InputHandler::glfwCursorPosCallback);
	glfwSetMouseButtonCallback(window, InputHandler::glfwMouseButtonCallback);
}

void Application::initGLEW()
{
	//init glew, return if fails. NOTE: this must be called after window is created since it requires opengl info(?)
	bool glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (GLEW_OK != err) {
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		throw std::runtime_error("glew init failed");
	}

	cout << "\nOpenGL and system informations." << endl;
	// print out some info about the graphics drivers
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl << endl;

	//@warning Hardcorded
	if (!GLEW_VERSION_4_3) {
		throw std::runtime_error("OpenGL 4.1 API is not available");
	}
}

void Application::initOpenGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
}

void Voxel::Application::initMainCamera()
{
	//Camera::mainCamera = Camera::create(vec3(0), 70.0f, 0.03f, 200.0f, 1280.0f / 720.0f);
	//Camera::mainCamera->addAngle(glm::vec3(0));
	Camera::mainCamera = Camera::create(vec3(0, 0, -20), 70.0f, 0.03f, 200.0f, 1280.0f / 720.0f);
	Camera::mainCamera->addAngle(glm::vec3(0, 180, 0));
}

void Voxel::Application::initWorld()
{
	world = new World();
}

void Voxel::Application::initTime()
{
	lastTime = static_cast<float>(glfwGetTime());
	elapsedTime = 0;
}

void Application::updateTime()
{
	float curTime = static_cast<float>(glfwGetTime());
	elapsedTime = curTime - lastTime;
	lastTime = curTime;
}

void Application::run()
{
	// reset cursor
	glfwSetCursorPos(window, 0, 0);
	auto& input = InputHandler::getInstance();
	input.setCursorToCenter();
	input.initControllerManager();
	input.update();
	
	while (!glfwWindowShouldClose(window))
	{
		updateTime();

		input.update();

		world->update(elapsedTime);

		world->render(elapsedTime);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}

void Application::glfwErrorCallback(int error, const char * description)
{
	std::cout << "[GLFW] Error: " << std::string(description) << std::endl;
}
