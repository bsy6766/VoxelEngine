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

		// Screen resolution
		int screenWidth;
		int screenHeight;
		std::string windowTitle;

		// Default program
		Program* defaultProgram;

		// GLFW callback func
		static void glfwErrorCallback(int error, const char* description);

		void init(const int screenWidth, const int screenHeight, const std::string& windowTitle, const int windowMode, const bool vsync); 
		
		void initGLFW();
		void initWindow(const int screenWidth, const int screenHeight, const std::string& windowTitle, const int windowMode, const bool vsync);
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
		void setWindowPosition(int x, int y);
		
		bool isWindowedFullScreen();
		void setWindowedFullScreen();
		void setWindowedFullScreen(const int monitorIndex);
		void setWindowedFullScreen(GLFWmonitor* monitor);

		bool isWindowDecorated();

		void close();
	};
}

#endif