#ifndef GLVIEW_H
#define GLVIEW_H

// cpp
#include <string>
#include <iostream>
#include <functional>

// glm
#include <glm\glm.hpp>

// gl
#include <GL\glew.h>
#include <GLFW\glfw3.h>

// voxel
#include "Config.h"

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

		// Checks if window is fullscreen
		bool isFullScreen();

		// Set window to fullscreen on primary monitor
		void setFullScreen();

		// Set window to fullscreen on desired monitor index. If monitor doesn't exists, uses primary monitor
		void setFullScreen(const int monitorIndex);

		// Set window to fullscreen on specific monitor
		void setFullScreen(GLFWmonitor* monitor);
		
		// Get GLFWmonitor with specific monitor index
		GLFWmonitor* getMonitorFromIndex(const int monitorIndex);

		// Check if window is windowed
		bool isWindowed();

		// Set window to windowed mode with specific size of resolution. Resolution must be greater than 0
		void setWindowed(int width, int height);

		// Set window's position to desired position. Origin is at primary monitor's top left corner.
		void setWindowPosition(int x, int y);
		
		// Check if window is windowed fullscreen mode
		bool isWindowedFullScreen();

		// Set window to windowed fullscreen on primary monitor
		void setWindowedFullScreen();

		// Set window to windowed fullscreen on desired monitor index. If monitor doesn't exists, uses primary monitor
		void setWindowedFullScreen(const int monitorIndex);

		// Set window to windowed fullscreen on specific monitor
		void setWindowedFullScreen(GLFWmonitor* monitor);

		// Set vsync mode. true to enable vsync, false to disable.
		void setVsync(const bool vsync);

		// Check if window is decoreated (has window menu and border)
		bool isWindowDecorated();

		// Set window's decoration. true to enable, false to disable.
		void setWindowDecoration(const bool mode);

		// Set window's floating. For debug
		void setWindowFloating(const bool mode);

		// Get screen size (resolution)
		glm::ivec2 getScreenSize();

		// Get screen size with glfw call.
		glm::ivec2 getGLFWWindowSize();

		// Set window's size to specific size regardless of window mode
		void setWindowSize(const int width, const int height);

		// Check if vsync is enabled or not.
		bool isVsyncEnabled();

		// Set opengl's buffer clear color. Color value must be between 0 to 1
		void setClearColor(const glm::vec3& color);

		// Set glfw cursor mode.
		void setCursorMode(const int mode);

		// Close glfw window
		void close();

		// FPS callback
		static std::function<void(int)> onFPSCounted;

		// get system info
		std::string getCPUName();
		std::string getGLVersion();
		std::string getGPUVendor();
		std::string getGPURenderer();


#if V_DEBUG
#if V_DEBUG_COUNT_VISIBLE_VERTICES
		// total vert
		bool countVertices;
		unsigned int totalVertices;

		bool doesCountVerticesSize();
		int getTotalVerticesSize();
		void addVerticesSize(const int size);
#endif
#if V_DEBUG_COUNT_VISIBLE_VERTICES
		// Draw calls
		bool countDrawCalls;
		unsigned int totalDrawCalls;

		bool doesCountDrawCalls();
		void incrementDrawCall();

		int getTotalDrawCalls();
#endif
#endif

	};
}

#endif