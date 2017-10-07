#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <iostream>
#include <GLView.h>

//#define GLM_FORCE_DEGREES
//#define GLM_PRECISION_MEDIUMP_FLOAT

namespace Voxel
{
	// Foward
	class Game;
	class DataTree;

	class Application
	{
	private:
		// Delete copy, move, assign operators
		Application(Application const&) = delete;             // Copy construct
		Application(Application&&) = delete;                  // Move construct
		Application& operator=(Application const&) = delete;  // Copy assign
		Application& operator=(Application &&) = delete;      // Move assign
	private:
		// Consturctor and Destructor. 
		Application();
		~Application();

		// OpenGL
		GLView* glView;

		// Initialize functions
		void initGLView();
		void initMainCamera();
		void initGame();
		void initInternalSettings();

		// Path to working directory
		std::string workingDirectory;

		// game
		Game* game;

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

		// Get OpenGL
		GLView* getGLView();

		// Get game
		Game* getGame();

		// Get working directory
		std::string getWorkingDirectory();
	};
}

#endif