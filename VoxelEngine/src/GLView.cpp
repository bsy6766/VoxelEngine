#include "GLView.h"

#include <InputHandler.h>
#include <ShaderManager.h>
#include <ProgramManager.h>
#include <Shader.h>
#include <Program.h>
#include <Color.h>

using namespace Voxel;
using std::cout;
using std::endl;

GLView::GLView()
	: window(nullptr)
	, defaultProgram(nullptr)
	, currentTime(0)
	, previousTime(0)
	, elapsedTime(0)
	, fps(0)
	, fpsElapsedTime(0)
	, fpsDisplay(false)
	, fpsKeyDown(false)
{
}

GLView::~GLView()
{
	// Delete all shaders
	ShaderManager::getInstance().releaseAll();
	ProgramManager::getInstance().releaseAll();

	if (window)
	{
		glfwDestroyWindow(window);
		window = nullptr;
	}

	glfwTerminate();
}

void Voxel::GLView::init()
{
	initGLFW();
	initWindow();
	initGLEW();
	initOpenGL();

	initDefaultShaderProgram();

	glfwSetCursorPos(window, 0, 0);
}

void Voxel::GLView::initGLFW()
{
	if (glfwInit() != GL_TRUE)
	{
		// GLFW failed to initailize.
		throw std::runtime_error("[GLView] Error. Failed to initialize GLFW");
	}
	else
	{
		// Initialzied GLFW.
		int versionMajor, versionMinor, versionRev;
		glfwGetVersion(&versionMajor, &versionMinor, &versionRev);

		cout << "[GLView] Intialized GLFW." << endl;
		cout << "[GLView] GLFW version " << versionMajor << "." << versionMinor << "." << versionRev << endl;
	}
}

void Voxel::GLView::initWindow()
{//set to OpenGL 4.3
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
	glfwSetWindowPos(window, 100 - 1920, 100);

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

void Voxel::GLView::initGLEW()
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
		throw std::runtime_error("OpenGL 4.3 API is not available");
	}
}

void Voxel::GLView::initOpenGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
}

void Voxel::GLView::initDefaultShaderProgram()
{
	auto vertexShader = ShaderManager::getInstance().createShader("defaultVert", "shaders/defaultVertexShader.glsl", GL_VERTEX_SHADER);
	auto fragmentShader = ShaderManager::getInstance().createShader("defaultFrag", "shaders/defaultFragmentShader.glsl", GL_FRAGMENT_SHADER);
	defaultProgram = ProgramManager::getInstance().createProgram("defaultProgram", vertexShader, fragmentShader);
}

bool Voxel::GLView::isRunning()
{
	return !glfwWindowShouldClose(window);
}

void Voxel::GLView::clearBuffer()
{
	auto skyboxColor = Color::SKYBOX;
	glClearColor(skyboxColor.x, skyboxColor.y, skyboxColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(defaultProgram->getObject());
}

void Voxel::GLView::render()
{
	glfwSwapBuffers(window);

	glfwPollEvents();

	glUseProgram(0);
}

void Voxel::GLView::updateTime()
{
	previousTime = currentTime;
	currentTime = glfwGetTime();

	elapsedTime = currentTime - previousTime;
}

void Voxel::GLView::updateFPS()
{
	fpsElapsedTime += elapsedTime;
	fps++;

	if (fpsKeyDown == false && glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		fpsKeyDown = true;
	}
	else if (fpsKeyDown && glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		fpsKeyDown = false;
		fpsDisplay = !fpsDisplay;
	}

	if (fpsElapsedTime > 1.0)
	{
		if (fpsDisplay)
		{
			std::cout << "Fps: " << fps << std::endl;
		}

		fps = 0;
		fpsElapsedTime -= 1.0;
	}
}

void Voxel::GLView::setCursorPos(double x, double y)
{
	glfwSetCursorPos(window, x, y);
}

void Voxel::GLView::resetTime()
{
	currentTime = previousTime = elapsedTime = 0;
}

int Voxel::GLView::getFPS()
{
	return fps;
}

double Voxel::GLView::getElaspedTime()
{
	return elapsedTime;
}

void Voxel::GLView::setFPSDisplay(const bool mode)
{
	fpsDisplay = mode;
}

void Voxel::GLView::close()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}

void GLView::glfwErrorCallback(int error, const char * description)
{
	std::cout << "[GLFW] Error: " << std::string(description) << std::endl;
}
