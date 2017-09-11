#ifndef GLVIEW_H
#define GLVIEW_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <iostream>
#include <functional>

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

		// vsync
		bool vsync;

		// GL clear color
		glm::vec3 clearColor;

		// Screen resolution
		int screenWidth;
		int screenHeight;
		std::string windowTitle;

		// System info
		std::string GLVersion;
		std::string GPUVendor;
		std::string GPURenderer;
		std::string CPUName;

		// GLFW callback func
		static void glfwErrorCallback(int error, const char* description);

		void init(const int screenWidth, const int screenHeight, const std::string& windowTitle, const int windowMode, const bool vsync); 
		
		void initGLFW();
		void initWindow(const int screenWidth, const int screenHeight, const std::string& windowTitle, const int windowMode, const bool vsync);
		void initGLEW();
		void initOpenGL();
		void initDefaultShaderProgram();
		void initCPUName();

		bool isRunning();

		void clearBuffer();
		void render();

		void updateTime();
		void updateFPS();
		void updateMousePosition(double& x, double& y);
	public:
		void setCursorPos(double x, double y);
		void resetTime();
		int getFPS();
		double getElaspedTime();

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

		void setVsync(const bool vsync);

		bool isWindowDecorated();

		glm::ivec2 getScreenSize();

		bool isVsyncEnabled();

		void setClearColor(const glm::vec3& color);

		void close();

		// FPS callback
		static std::function<void(int)> onFPSCounted;

		// get system info
		std::string getCPUName();
		std::string getGLVersion();
		std::string getGPUVendor();
		std::string getGPURenderer();
	};
}

#endif