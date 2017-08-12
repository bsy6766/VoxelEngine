#include "Application.h"

#include <iostream>
#include <glm\glm.hpp>

using std::cout;
using std::endl;

Application::Application()
{
	cout << "Creating Application" << endl;
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
}

void Application::init()
{
	initGLFW();
	initWindow();
	initGLEW();
	initOpenGL();
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

	// Set callbacks
	glfwSetErrorCallback(glfwErrorCallback);
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, glfwKeyCallback);
	glfwSetCursorPosCallback(window, glfwCursorPosCallback);
	glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
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

}

void Application::run()
{
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Application::glfwErrorCallback(int error, const char * description)
{
}

void Application::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}

void Application::glfwCursorPosCallback(GLFWwindow* window, double x, double y)
{
}

void Application::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}
