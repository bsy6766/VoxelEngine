#ifndef GLVIEW_H
#define GLVIEW_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <iostream>

namespace Voxel
{
	class Program;

	/**
	*	@class GLView
	*	
	*	GLView class manages OpenGL context with GLFW.
	*/
	class GLView
	{
		friend class Application;
	private:
		// Only application can create GLView.
		GLView();
		~GLView();

		// Window
		GLFWwindow* window;
		GLFWmonitor* monitor;

		// Time & FPS
		double currentTime;
		double previousTime;
		double elapsedTime;
		int fps;
		double fpsElapsedTime;
		bool fpsDisplay;
		bool fpsKeyDown;

		// Default program
		Program* defaultProgram;

		// GLFW callback func
		static void glfwErrorCallback(int error, const char* description);

		void init(); 
		
		void initGLFW();
		void initWindow();
		void initGLEW();
		void initOpenGL();
		void initDefaultShaderProgram();

		bool isRunning();

		void clearBuffer();
		void render();

		void updateTime();
		void updateFPS();
	public:
		void setCursorPos(double x, double y);
		void resetTime();
		int getFPS();
		double getElaspedTime();

		void setFPSDisplay(const bool mode);

		bool isFullScreen();
		void setFullScreen();
		void setFullScreen(const int monitorIndex);
		void setFullScreen(GLFWmonitor* monitor);

		GLFWmonitor* getMonitorFromIndex(const int monitorIndex);

		bool isWindowed();
		void setWindowed(int width, int height);
		
		bool isWindowedFullScreen();
		void setWindowedFullScreen();
		void setWindowedFullScreen(const int monitorIndex);
		void setWindowedFullScreen(GLFWmonitor* monitor);

		bool isWindowDecorated();

		void close();
	};
}

#endif