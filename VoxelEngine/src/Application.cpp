#include "Application.h"

#include <iostream>

#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>

#include <ShaderManager.h>
#include <ProgramManager.h>

#include <InputHandler.h>
#include <functional>

// temp
#include <Shader.h>
#include <Program.h>
#include <Camera.h>
#include <glm\gtx\transform.hpp>
#include <Cube.h>

using std::cout;
using std::endl;
using namespace Voxel;

Application::Application()
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

	if (camera)
	{
		delete camera;
	}
}

void Application::init()
{
	initGLFW();
	initWindow();
	initGLEW();
	initOpenGL();

	camera = Camera::create(vec3(0, 0, -10.0f), 90.0f, 0.001f, 50.0f, 1280.0f / 720.0f);
	//camera->setPosition(vec3(0, 0, 0));
	lastTime = 0;
	auto vertexShader = ShaderManager::getInstance().createShader("defaultVert", "shaders/defaultVertexShader.glsl", GL_VERTEX_SHADER);
	auto fragmentShader = ShaderManager::getInstance().createShader("defaultFrag", "shaders/defaultFragmentShader.glsl", GL_FRAGMENT_SHADER);
	program = ProgramManager::getInstance().createProgram("defaultProgram", vertexShader, fragmentShader);

	// Generate vertex array object
	glGenVertexArrays(1, &vao);
	// Bind it
	glBindVertexArray(vao);

	// Generate buffer object
	glGenBuffers(1, &vbo);
	// Bind it
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Get cube verticies and indicies
	std::vector<float> cubeVerticies = Cube::getVerticies(Cube::Face::ALL, 1.0f, 0.0f, 0.0f);
	std::vector<unsigned int> cubeIndicies = Cube::getIndicies(Cube::Face::ALL);

	// Load cube verticies
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticies) * cubeVerticies.size(), &cubeVerticies[0], GL_STATIC_DRAW);
	// Enable verticies attrib
	GLint vertLoc = program->getAttribLocation("vert");
	GLint colorLoc = program->getAttribLocation("color");
	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
	// unbind buffer
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate indicies object
	glGenBuffers(1, &ibo);
	// Bind indicies
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// Load indicies
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndicies) * cubeIndicies.size(), &cubeIndicies[0], GL_STATIC_DRAW);
	// unbind buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
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
}

void Application::run()
{
	while (!glfwWindowShouldClose(window))
	{
		auto cur = glfwGetTime();
		auto elapsed = cur - lastTime;
		lastTime = cur;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program->getObject());

		float speed = 0.3f;
		//tempRotation = glm::rotate(tempRotation, speed * static_cast<float>(elapsed), vec3(1, 0, 0));
		//tempRotation = glm::rotate(tempRotation, speed * static_cast<float>(elapsed), vec3(0, 1, 0));
		//tempRotation = glm::rotate(tempRotation, speed * static_cast<float>(elapsed), vec3(0, 0, 1));

		program->setUniformMat4("cameraMat", camera->getMatrix());
		program->setUniformMat4("modelMat", tempRotation);

		glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Voxel::Application::onMouseMove(double x, double y)
{
	{
		std::cout << "cursor pos " << x << ", " << y << std::endl;
	}
}

void Application::glfwErrorCallback(int error, const char * description)
{
}
