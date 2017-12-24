#ifndef GLVIEW_H
#define GLVIEW_H

// cpp
#include <string>
#include <iostream>
#include <functional>
#include <vector>

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
	*	@struct VideoMode
	*	@brief A simple pack of data that has GLFW video mode data
	*/
	struct VideoMode
	{
		// Only GLView class can access this struct
		friend class GLView;
	public:
		int width;
		int height;
		int redBits;
		int greenBits;
		int blueBits;
		int refreshRate;

	private:
		// constructor
		VideoMode() : width(0), height(0), redBits(0), greenBits(0), blueBits(0), refreshRate(0) {}
		VideoMode(const GLFWvidmode& glfwVideoMode) : width(glfwVideoMode.width), height(glfwVideoMode.height), redBits(glfwVideoMode.redBits), greenBits(glfwVideoMode.greenBits), blueBits(glfwVideoMode.blueBits), refreshRate(glfwVideoMode.refreshRate) {}

	public:
		glm::ivec2 getResolution() const { return glm::ivec2(width, height); }
		glm::ivec3 getRGBBits() const { return glm::ivec3(redBits, greenBits, blueBits); }
		int getRefreshRate() const { return refreshRate; }
	};

	/**
	*	@struct Monitor
	*	@brief A simple pack of that that has monitor info with list of video mode
	*/
	struct MonitorInfo
	{
		// Only GLView class can access this struct
		friend class GLView;
	private:
		// Constructor
		MonitorInfo() : name(""), index(-11) {}

		// name of monitor
		std::string name;

		// index of monitor
		int index;

		// list of video mode. Small -> Large
		std::vector<VideoMode> videoModes;
	public:
		~MonitorInfo() = default;

		// get name
		std::string getName() const { return name; }
		int getIndex() const { return index; }
		std::vector<VideoMode> getVideoModes() const { return videoModes; }
	};

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

		// Monitos. @see Monitor for details
		std::vector<MonitorInfo> monitors;

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
		void init(const std::string& windowTitle); 
		
		/**
		*	Initialize GLFW
		*/
		void initGLFW();

		/**
		*	Initialize monitor data
		*/
		void initMonitorData();

		/**
		*	Initailize GLFW window
		*/
		void initWindow(const int screenWidth, const int screenHeight, const std::string& windowTitle, const int monitorIndex, const int windowMode, const bool vsync);

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
		void initShaderPrograms();

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
		*	Clear rendering setting to initial state.
		*	Need to call this before the rendering loop starts.
		*/
		void clearRender();

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

		// Get monitor info
		MonitorInfo* getMonitorInfo(const unsigned int monitorInfoIndex);

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

		// Set window minimized
		void setWindowMinimized();

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

		// calibrate time.
		void calibrateTime();

		// Close glfw window
		void close();

		// FPS callback
		std::function<void(int)> onFPSCounted;

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