#ifndef APPLICATION_H
#define APPLICATION_H

#include <GL\glew.h>
#include <GLFW\glfw3.h>

class Application
{
private:
	GLFWwindow* window;

	void initGLFW();
	void initWindow();
	void initGLEW();
	void initOpenGL();

	// GLFW callback func
	static void glfwErrorCallback(int error, const char* description);
	static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfwCursorPosCallback(GLFWwindow* window, double x, double y);
	static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
public:
	Application();
	~Application();

	void init();
	void run();
};

#endif