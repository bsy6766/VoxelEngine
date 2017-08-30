#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <glm\glm.hpp>

namespace Voxel
{
	namespace UI
	{
		class Canvas;
		class Text;
		class Image;
	}

	class DebugConsole
	{
	private:
		bool openingConsole;
		bool debugOutputVisibility;

		UI::Canvas* debugCanvas;

		UI::Text* staticLabels;
		UI::Text* fpsNumber;
		UI::Text* resolutionNumber;
		UI::Text* vsyncMode;

		UI::Text* mousePosition;
		UI::Text* cameraPosition;
		UI::Text* cameraRotation;

		UI::Text* playerPosition;
		UI::Text* playerChunkPosition;
		UI::Text* playerLookingAt;

		UI::Text* chunkNumbers;

		UI::Image* commandInputField;
	public:
		DebugConsole();
		~DebugConsole();
		 
		void init();

		void openConsole();
		void closeConsole();

		bool isConsoleOpened();

		void feedKeyboardInput(const std::string& c);

		void render();

		void toggleDubugOutputs();

		void onFPSUpdate(int fps);
		
		void updateResolution(int width, int height);
		void updateVsync(bool vsync);
		void updatePlayerPosition(const glm::vec3& position);
		void updatePlayerLookingAt(const glm::ivec3& lookingAt);
		void updateChunkNumbers(const int visible, const int active, const int total);
		void updateMousePosition();
	};
}

#endif