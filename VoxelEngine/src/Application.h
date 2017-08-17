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
		Application();
		~Application();

		// OpenGL
		GLView* glView;

		void initGLView();
		void initMainCamera();
		void initWorld();
		void initConfig();

		// wd
		std::string workingDirectory;

		// world
		World* world;

		DataTree* configData;

		// Clean up
		void cleanUp();
	public:
		static Application& getInstance()
		{
			static Application instance;
			return instance;
		}

		void init();
		void run();

		void end();

		GLView* getGLView();
	};
}

#endif