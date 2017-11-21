#include "GLView.h"

#include <InputHandler.h>
#include <ShaderManager.h>
#include <ProgramManager.h>
#include <Shader.h>
#include <Program.h>

#include <sstream>

#include <Camera.h>

using namespace Voxel;
using std::cout;
using std::endl;

std::function<void(int)> GLView::onFPSCounted = nullptr;

GLView::GLView()
	: window(nullptr)
	, currentTime(0)
	, previousTime(0)
	, elapsedTime(0)
	, fps(0)
	, fpsElapsedTime(0)
	, monitor(nullptr)
	, screenWidth(0)
	, screenHeight(0)
	, windowTitle("")
	, clearColor(0)
	, vsync(false)
	, countDrawCalls(true)
	, totalDrawCalls(0)
	, countVertices(true)
	, totalVertices(0)
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
	initCPUName();
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

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	auto w = videoMode->width;
	auto h = videoMode->height;

	if (windowMode == 0)
	{
		// windowed. Must be decorated.
		// Auto iconify is ignored
		glfwWindowHint(GLFW_DECORATED, GL_TRUE);
	}
	else if (windowMode == 1)
	{
		// fullscreen
		// Must auto iconify.
		// Decoration is ignored.
		// Start from primary monitor
		// Todo: Save monitor location. If valid, open window on saved monitor. Else, start from primary window
		glfwWindowHint(GLFW_AUTO_ICONIFY, GL_TRUE);
		monitor = glfwGetPrimaryMonitor();
	}
	else if (windowMode == 2)
	{
		// borderless fullscreen
		// A windowed, size of monitor.
		// No decoration
		// Auto iconify is ignored
		glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	}
	else
	{
		// Forcing windowed to 1280 x 720 resolution
		glfwWindowHint(GLFW_DECORATED, GL_TRUE);
		this->screenWidth = 1280;
		this->screenHeight = 720;
	}

	window = glfwCreateWindow(this->screenWidth, this->screenHeight, windowTitle.c_str(), monitor, nullptr);

	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("GLFW failed to create window");
	}

	// if window successfully made, make it current window
	glfwMakeContextCurrent(window);
	
	/*
	*	Vsync. 
	*	Vsync synchronizes game's frame rate to monitors frame rate.
	*
	*	Notes on vsync...
	*	Setting glfwSwapInterval(1) enables GLFW vsync and gives 60 fps on my monitor.
	*	Setting glfwSwapInterval(0) disables GLFW vsync, but DOES NOT switch vsync off (http://www.glfw.org/docs/latest/group__context.html#ga6d4e0cdf151b5e579bd67f13202994ed).
	*
	*	There is a screen tearing in the game while window is windowed fullscreen mode.
	*	Here's some researches that I have done so far.
	*	In Windows 10, there are mutiple ways to use vsync.
	*	First, you can decide on application level. (glfwSwapInterval I guess)
	*	Second, graphics card can change the vsync (In nvidia control panel with more option)
	*	Last, OS (Windows 10 in my case) overrides the vsync seeting.
	*
	*	Windows 10
	*	WIndows 10 uses DWM (Display Window Manager). DWM manages all the windows on the screen.
	*	This includes windowed mode and windowed fullscreen. 
	*	Like said, all windowed application is managed by DWM and will forced to triple buffer.
	*	Only exclusive fullscreen will override DWM by graphics card. This means that fullscreen's vsync won't be affected by OS.
	*	Meanwhile, windowed has some issue with vsync, especially while windowed window's size equal to monitor size and it's on primary monitor(with taskbar)
	*	Tearing happens when the vsync is applied and that's basically it. It happens to lots of game and people are confused too.
	*	
	*	For now, I'm just going to let user decide to use vsync or not.
	*	Best option is fullscreen with desired vsync mode, windowed without vsync (no tearing with vsync though) or windowed fullscreen without vsyc (tearing with vsync)
	*/
	this->vsync = vsync;

	setVsync(vsync);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set callbacks
	glfwSetErrorCallback(glfwErrorCallback);
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, InputHandler::glfwKeyCallback);
	glfwSetCursorPosCallback(window, InputHandler::glfwCursorPosCallback);
	glfwSetMouseButtonCallback(window, InputHandler::glfwMouseButtonCallback);
	glfwSetScrollCallback(window, InputHandler::glfwScrollCallback);
	glfwSetWindowFocusCallback(window, GLView::glfwWindowFocusCallback);
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

	cout << "\n[GLView] OpenGL and GPU informations" << endl;

	std::stringstream vss;
	vss << glGetString(GL_VENDOR);

	GPUVendor = vss.str();

	std::stringstream rss;
	rss << glGetString(GL_RENDERER);

	GPURenderer = rss.str();

	std::stringstream glss;
	glss << glGetString(GL_VERSION);

	GLVersion = glss.str();

	std::cout << "[GLView] OpenGL version: " << GLVersion << std::endl;
	std::cout << "[GLView] GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "[GLView] GPU vendor: " << GPUVendor << std::endl;
	std::cout << "[GLView] GPU renderer: " << GPURenderer << std::endl;

	//@warning Hardcorded
	if (!GLEW_VERSION_4_3) {
		throw std::runtime_error("OpenGL 4.3 API is not available");
	}
}

void Voxel::GLView::initOpenGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_DITHER);

	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Voxel::GLView::initDefaultShaderProgram()
{
	ProgramManager::getInstance().initDefaultPrograms();
}

void Voxel::GLView::initCPUName()
{
	// Get extended ids.
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];

	// Get the information associated with each extended ID.
	char CPUBrandString[0x40] = { 0 };
	for (unsigned int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);

		// Interpret CPU brand string and cache information.
		if (i == 0x80000002)
		{
			memcpy(CPUBrandString,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000003)
		{
			memcpy(CPUBrandString + 16,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000004)
		{
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
	}

	CPUName = std::string(CPUBrandString);

	auto first = CPUName.find_first_not_of(' ');
	CPUName = CPUName.substr(first, CPUName.size());

	std::cout << "[GLView] CPU: " << CPUName << std::endl;
}

bool Voxel::GLView::isRunning()
{
	return !glfwWindowShouldClose(window);
}

void Voxel::GLView::clearBufferBit()
{
	glClearColor(clearColor.r, clearColor.g, clearColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);
}

void Voxel::GLView::render()
{
	glfwSwapBuffers(window);

	glfwPollEvents();

	glUseProgram(0);

	totalDrawCalls = 0;
	totalVertices = 0;
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

	if (fpsElapsedTime > 1.0)
	{
		if (onFPSCounted)
		{
			onFPSCounted(fps);
		}

		fps = 0;
		fpsElapsedTime -= 1.0;
	}
}

void Voxel::GLView::updateMousePosition(double& x, double& y)
{
	glfwGetCursorPos(window, &x, &y);
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

	screenWidth = w;
	screenHeight = h;

	//glfwSetWindowAttrib(window, GLFW_AUTO_ICONIFY, GLFW_TRUE);
	glfwSetWindowMonitor(window, monitor, 0, 0, w, h, videoMode->refreshRate);
	glViewport(0, 0, w, h);
	/*
	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	auto w = videoMode->width;
	auto h = videoMode->height;
	glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, w, h, videoMode->refreshRate);
	glViewport(0, 0, w, h);
	*/

	Camera::mainCamera->updateScreenSizeAndAspect(static_cast<float>(w), static_cast<float>(h));
}

void Voxel::GLView::reopenWindow()
{
	glfwDestroyWindow(window);
	initWindow(1920, 1080, "reopend", 1, false);
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

		xpos += (videoMode->width - width) / 2;
		ypos += (videoMode->height - height) / 2;

		monitor = nullptr;

		screenWidth = width;
		screenHeight = height;

		glfwSetWindowMonitor(window, nullptr, xpos, ypos, width, height, GLFW_DONT_CARE);

		int newWidth, newHeight;
		glfwGetWindowSize(window, &newWidth, &newHeight);

		std::cout << "nw = " << newWidth << ", nh = " << newHeight << std::endl;

		glViewport(0, 0, newWidth, newHeight);

		glfwSetWindowPos(window, 0, 31);

		/*
		glfwSetWindowMonitor(window, nullptr, 0, 0, width, height, GLFW_DONT_CARE);
		glViewport(0, 0, width, height);
		*/

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

	screenWidth = w;
	screenHeight = h;

	glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
	glfwSetWindowAttrib(window, GLFW_AUTO_ICONIFY, GLFW_FALSE);

	glfwSetWindowMonitor(window, nullptr, xpos, ypos, w, h, GLFW_DONT_CARE);
	
	glViewport(0, 0, w, h);

	/*
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
	glViewport(0, 0, mode->width, mode->height);
	*/

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

void Voxel::GLView::setWindowDecoration(const bool mode)
{
	glfwSetWindowAttrib(window, GLFW_DECORATED, mode);
}

void Voxel::GLView::setWindowFloating(const bool mode)
{
	glfwSetWindowAttrib(window, GLFW_FLOATING, mode);
}

bool Voxel::GLView::doesCountDrawCalls()
{
	return countDrawCalls;
}

bool Voxel::GLView::doesCountVerticesSize()
{
	return countVertices;
}

void Voxel::GLView::incrementDrawCall()
{
	totalDrawCalls++;
}

int Voxel::GLView::getTotalDrawCalls()
{
	return totalDrawCalls;
}

int Voxel::GLView::getTotalVerticesSize()
{
	return totalVertices;
}

void Voxel::GLView::addVerticesSize(const int size)
{
	if (size > 0)
	{
		totalVertices += size;
	}
}

glm::ivec2 Voxel::GLView::getScreenSize()
{
	return glm::ivec2(screenWidth, screenHeight);
}

glm::ivec2 Voxel::GLView::getGLFWWindowSize()
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);

	return glm::ivec2(w, h);
}

void Voxel::GLView::setWindowSize(const int width, const int height)
{
	if (isWindowed() || isWindowedFullScreen())
	{
	}
	std::cout << "[GLView] Changing window size to (" << width << ", " << height << ") in windowed mode\n";
	glfwSetWindowSize(window, width, height);
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

bool Voxel::GLView::isVsyncEnabled()
{
	return vsync;
}

void Voxel::GLView::setClearColor(const glm::vec3 & color)
{
	clearColor = color;
}

void Voxel::GLView::setCursorMode(const int mode)
{
	glfwSetInputMode(window, GLFW_CURSOR, mode);
}

void Voxel::GLView::close()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}

std::string Voxel::GLView::getCPUName()
{
	return CPUName;
}

std::string Voxel::GLView::getGLVersion()
{
	return GLVersion;
}

std::string Voxel::GLView::getGPUVendor()
{
	return GPUVendor;
}

std::string Voxel::GLView::getGPURenderer()
{
	return GPURenderer;
}

void GLView::glfwErrorCallback(int error, const char * description)
{
	std::cout << "[GLFW] Error: " << std::string(description) << std::endl;
}

void Voxel::GLView::glfwWindowFocusCallback(GLFWwindow * window, int focus)
{
	std::cout << "[GLView] ";
	if (focus == GLFW_TRUE)
	{
		std::cout << "Gained focus\n";
		Application::getInstance().skipFrame();
	}
	else
	{
		std::cout << "Lost focus\n";
	}
}
