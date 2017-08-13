#ifndef APPLICATION_H
#define APPLICATION_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <iostream>

#define GLM_FORCE_DEGREES

namespace Voxel
{
	class Program;
	class World;

	class Application
	{
	private:
		GLFWwindow* window;

		void initGLFW();
		void initWindow();
		void initGLEW();
		void initOpenGL();
		void initMainCamera();
		void initWorld();

		// GLFW callback func
		static void glfwErrorCallback(int error, const char* description);

		// wd
		std::string workingDirectory;

		// world
		World* world;

		// temp
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
		Program* program;
		double lastTime;
		glm::vec3 angle = glm::vec3(0);
		glm::mat4 tempRotation = glm::mat4(1.0f);
		glm::mat4 tempTralsnate = glm::mat4(1.0f);
	public:
		Application();
		~Application();

		void init();
		void run();

		void onMouseMove(double x, double y);
		void test(int i) {};
	};
}

#endif