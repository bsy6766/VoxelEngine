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

		// Draw calls
		bool countDrawCalls;
		unsigned int totalDrawCalls;

		// total vert
		bool countVertices;
		unsigned int totalVertices;

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
		static void glfwWindowFocusCallback(GLFWwindow* window, int focus);

		// Intiialize opengl
		void init(const int screenWidth, const int screenHeight, const std::string& windowTitle, const int windowMode, const bool vsync); 
		
		/**
		*	Initialize GLFW
		*/
		void initGLFW();

		/**
		*	Initailize GLFW window
		*/
		void initWindow(const int screenWidth, const int screenHeight, const std::string& windowTitle, const int windowMode, const bool vsync);

		/**
		*	Initialize GLEW
		*/
		void initGLEW();

		/**
		*	Enables OpenGL Attributes
		*/
		void initOpenGL();

		/**
		*	Initialize defulat shader programs that are used in game
		*/
		void initDefaultShaderProgram();

		/**
		*	Get CPU name for debugging
		*/
		void initCPUName();

		/**
		*	Check if OpenGL is running. Checks if GLFW window shoudl close or not
		*	@return True if window should be closed. Else, false
		*/
		bool isRunning();

		/**
		*	Clear OpenGL color and depth buffer bits
		*/
		void clearBufferBit();

		/**
		*	Render to screen.	
		*/
		void render();

		/**
		*	Update time. Calculates elapsed time on this frame using GLFW
		*/
		void updateTime();

		/**
		*	Update FPS.
		*/
		void updateFPS();

		/**
		*	Set cursor position.
		*	@param [in] x X position of cursor
		*	@param [in] y Y position of cursor
		*/
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

		void reopenWindow();

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
		void setWindowDecoration(const bool mode);

		void setWindowFloating(const bool mode);

		bool doesCountDrawCalls();
		bool doesCountVerticesSize();
		void incrementDrawCall();

		int getTotalDrawCalls();
		int getTotalVerticesSize();
		void addVerticesSize(const int size);

		glm::ivec2 getScreenSize();
		glm::ivec2 getGLFWWindowSize();

		void setWindowSize(const int width, const int height);

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