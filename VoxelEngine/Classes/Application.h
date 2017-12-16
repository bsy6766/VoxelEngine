#ifndef APPLICATION_H
#define APPLICATION_H

// cpp
#include <string>

// voxel
#include "GLView.h"
#include "Config.h"

namespace Voxel
{
	// Foward Declaration
	class DataTree;
	class Director;
	class Cursor;
	class Logger;

	/**
	*	@class Application
	*	@brief A root instance that runs the application.
	*
	*	Application is the root class that runs the entire application.
	*	Application initialize all required resources and default sub systems that the game uses.
	*
	*	Application has GLView instance that manages the OpenGL, GLFW and GLEW. @see class GLView for details.
	*	Application has Director instance that manages all the scenes in the game. @see class Director and Scene.
	*	
	*	Application runs main game (while) loop.
	*/
	class Application
	{
	private:
		// Delete copy, move, assign operators
		Application(Application const&) = delete;             // Copy construct
		Application(Application&&) = delete;                  // Move construct
		Application& operator=(Application const&) = delete;  // Copy assign
		Application& operator=(Application &&) = delete;      // Move assign
	private:
		// Consturctor
		Application();

		// Destructor
		~Application();

		// OpenGL
		GLView* glView;

		// Initialize GLView (OpenGL and GLEW)
		void initGLView();

		// Initialize main camera
		void initMainCamera();

		// Initailzie game
		void initDirector();

		// Init fonts
		void initFonts();

		// Init sprite sheet
		void initSpriteSheets();

		// Path to working directory
		std::string workingDirectory;
		
		// Director
		Director* director;

		// cursor. Cursor is part of system not scene.
		Cursor* cursor;

		// Set true to skip next frame.
		bool needToSkipFrame;
		
		// Clean up
		bool cleaned;
		void cleanUp();
	public:
		// Get application instance
		static Application& getInstance()
		{
			static Application instance;
			return instance;
		}

		// initialize 
		void init();

		// Run the main loop
		void run();

		// end the main loop
		void end();

		void skipFrame();

		// Get OpenGL
		GLView* getGLView();

		// Get director
		Director* getDirector();

		// Take screenshot
		bool takeScreenShot();
		
#if V_BUILD_NUMBER
		// Initialize internal settings
		void initInternalSettings();
#endif
	};
}

#endif