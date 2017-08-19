#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <iostream>
#include <GLView.h>

#define GLM_FORCE_DEGREES

namespace Voxel
{
	// Foward
	class World;
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
		void initWorld();
		void initConfig();

		// Path to working directory
		std::string workingDirectory;

		// world
		World* world;

		// Config
		DataTree* configData;

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

		// Get working directory
		std::string getWorkingDirectory();
	};
}

#endif