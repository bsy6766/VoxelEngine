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
	// Foward
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
		void initTime();

		void updateTime();

		// GLFW callback func
		static void glfwErrorCallback(int error, const char* description);

		// wd
		std::string workingDirectory;

		// world
		World* world;

		// Time
		float lastTime;
		float elapsedTime;
	public:
		Application();
		~Application();

		void init();
		void run();
	};
}

#endif