#ifndef APPLICATION_H
#define APPLICATION_H

// cpp
#include <string>

// voxel
#include "GLView.h"
#include "Config.h"

namespace Voxel
{
	// Foward
	class DataTree;
	class Director;
	class Cursor;

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

		// Initialize internal settings
		void initInternalSettings();

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

		// internal setting
		DataTree* internalSetting;

		// Clean up
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

		// Get working directory
		std::string getWorkingDirectory();
	};
}

#endif