#include "GLView.h"

#include <InputHandler.h>
#include <ShaderManager.h>
#include <ProgramManager.h>
#include <Shader.h>
#include <Program.h>

#include <Camera.h>

using namespace Voxel;
using std::cout;
using std::endl;

GLView::GLView()
	: window(nullptr)
	, currentTime(0)
	, previousTime(0)
	, elapsedTime(0)
	, fps(0)
	, fpsElapsedTime(0)
	, fpsDisplay(false)
	, fpsKeyDown(false)
	, monitor(nullptr)
	, screenWidth(0)
	, screenHeight(0)
	, windowTitle("")
	, clearColor(0)
	, vsync(false)
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

void Voxel::GLView::init(const int screenWidth, const int screenHeight, const std::string& windowTitle, const int windowMode, const bool vsync)
{
	initGLFW();
	initWindow(screenWidth, screenHeight, windowTitle, windowMode, vsync);
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

void Voxel::GLView::initWindow(const int screenWidth, const int screenHeight, const std::string& windowTitle, const int windowMode, const bool vsync)
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

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	monitor = nullptr;

	if (windowMode == 0)
	{
		// windowed
		glfwWindowHint(GLFW_DECORATED, GL_TRUE);
	}
	else if (windowMode == 1)
	{
		// fullscreen
		glfwWindowHint(GLFW_AUTO_ICONIFY, GL_TRUE);
	}
	else if (windowMode == 2)
	{
		// borderless fullscreen
		glfwWindowHint(GLFW_DECORATED, GL_FALSE);
		glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
	}
	else
	{
		// Forcing windowed to 1280 x 720 resolution
		glfwWindowHint(GLFW_DECORATED, GL_TRUE);
		this->screenWidth = 1280;
		this->screenHeight = 720;
	}

	window = glfwCreateWindow(this->screenWidth, this->screenHeight, windowTitle.c_str(), monitor, nullptr);
	//glfwSetWindowPos(window, 100 - 1920, 100);

	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("GLFW failed to create window");
	}

	// if window successfully made, make it current window
	glfwMakeContextCurrent(window);


	this->vsync = vsync;
	if (vsync)
	{
		glfwSwapInterval(1);
	}
	else
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
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glEnable(GL_LINE_SMOOTH);
	//GLfloat lineWidthRange[2] = { 0.0f, 0.0f };
	//glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);

	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Voxel::GLView::initDefaultShaderProgram()
{
	ProgramManager::getInstance().initDefaultPrograms();
}

bool Voxel::GLView::isRunning()
{
	return !glfwWindowShouldClose(window);
}

void Voxel::GLView::clearBuffer()
{
	glClearColor(clearColor.r, clearColor.g, clearColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);
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

bool Voxel::GLView::isFullScreen()
{
	return (monitor != nullptr && isWindowDecorated() == GLFW_FALSE);
}

void Voxel::GLView::setFullScreen()
{
	auto targetMonitor = glfwGetPrimaryMonitor();

	if (targetMonitor == nullptr)
	{
		this->monitor = nullptr;
		return;
	}

	setFullScreen(targetMonitor);
}

void Voxel::GLView::setFullScreen(const int monitorIndex)
{
	auto targetMonitor = getMonitorFromIndex(monitorIndex);
	if (targetMonitor == nullptr)
	{
		this->monitor = nullptr;
		return;
	}
	else
	{
		setFullScreen(targetMonitor);
	}
}

void Voxel::GLView::setFullScreen(GLFWmonitor * monitor)
{
	if (monitor == nullptr)
	{
		this->monitor = nullptr;
		return;
	}

	this->monitor = monitor;

	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

	auto w = videoMode->width;
	auto h = videoMode->height;	

	glfwSetWindowMonitor(window, monitor, 0, 0, w, h, videoMode->refreshRate);
	glViewport(0, 0, w, h);
	Camera::mainCamera->updateScreenSizeAndAspect(static_cast<float>(w), static_cast<float>(h));
}

GLFWmonitor * Voxel::GLView::getMonitorFromIndex(const int monitorIndex)
{
	int count = 0;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	if (monitorIndex < 0 || monitorIndex >= count)
	{
		return nullptr;
	}
	else
	{
		return monitors[monitorIndex];
	}
}

bool Voxel::GLView::isWindowed()
{
	return (monitor == nullptr && isWindowDecorated() == GLFW_TRUE);
}

void Voxel::GLView::setWindowed(int width, int height)
{
	if (isWindowed())
	{
		glfwSetWindowSize(window, width, height);
		glViewport(0, 0, width, height);
	}
	else
	{
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);

		if (monitor == nullptr)
		{
			monitor = glfwGetPrimaryMonitor();
		}

		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

		int xpos = 0, ypos = 0;
		glfwGetMonitorPos(monitor, &xpos, &ypos);
		xpos += (videoMode->width - width) * 0.5;
		ypos += (videoMode->height - height) * 0.5;

		monitor = nullptr;

		glfwSetWindowMonitor(window, monitor, xpos, ypos, width, height, GLFW_DONT_CARE);
		glViewport(0, 0, width, height);
		Camera::mainCamera->updateScreenSizeAndAspect(static_cast<float>(width), static_cast<float>(height));
	}
}

void Voxel::GLView::setWindowPosition(int x, int y)
{
	glfwSetWindowPos(window, x, y);
}

bool Voxel::GLView::isWindowedFullScreen()
{
	return glfwGetWindowMonitor(window) == nullptr && isWindowDecorated() == GLFW_FALSE;
}

void Voxel::GLView::setWindowedFullScreen()
{
	monitor = glfwGetPrimaryMonitor();

	if (monitor == nullptr)
	{
		return;
	}
	else
	{
		setWindowedFullScreen(monitor);
	}
}

void Voxel::GLView::setWindowedFullScreen(const int monitorIndex)
{
	auto targetMonitor = getMonitorFromIndex(monitorIndex);

	if (targetMonitor == nullptr)
	{
		return;
	}
	else
	{
		setWindowedFullScreen(targetMonitor);
	}
}

void Voxel::GLView::setWindowedFullScreen(GLFWmonitor * monitor)
{
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

	int xpos = 0, ypos = 0;
	glfwGetMonitorPos(monitor, &xpos, &ypos);

	auto w = videoMode->width;
	auto h = videoMode->height;

	glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
	glfwSetWindowMonitor(window, nullptr, xpos, ypos, w, h, videoMode->refreshRate);
	glViewport(0, 0, w, h);
	Camera::mainCamera->updateScreenSizeAndAspect(static_cast<float>(w), static_cast<float>(h));
}

void Voxel::GLView::setVsync(const bool vsync)
{
	this->vsync = vsync;

	if (vsync)
	{
		glfwSwapInterval(1);
	}
	else
	{
		glfwSwapInterval(0);
	}
}

bool Voxel::GLView::isWindowDecorated()
{
	return glfwGetWindowAttrib(window, GLFW_DECORATED);
}

glm::ivec2 Voxel::GLView::getScreenSize()
{
	return glm::ivec2(screenWidth, screenHeight);
}

void Voxel::GLView::setClearColor(const glm::vec3 & color)
{
	clearColor = color;
}

void Voxel::GLView::close()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}

void GLView::glfwErrorCallback(int error, const char * description)
{
	std::cout << "[GLFW] Error: " << std::string(description) << std::endl;
}
