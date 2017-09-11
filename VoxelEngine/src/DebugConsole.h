#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <glm\glm.hpp>
#include <Cube.h>

namespace Voxel
{
	namespace UI
	{
		class Canvas;
		class Text;
		class Image;
	}

	class Player;
	class World;

	class DebugConsole
	{
	private:
		bool openingConsole;
		bool debugOutputVisibility;

		const std::string DefaultCommandInputText = "ENTER COMMAND";

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
		UI::Text* command;

		void executeCommand(const std::string& command);
	public:
		DebugConsole();
		~DebugConsole();

		// instances for debug
		Player* player;
		World* world;
		 
		void init();

		void openConsole();
		void closeConsole();

		bool isConsoleOpened();

		void updateConsoleInputText(const std::string& c);

		void render();

		void toggleDubugOutputs();

		void onFPSUpdate(int fps);
		
		void updateResolution(int width, int height);
		void updateVsync(bool vsync);
		void updatePlayerPosition(const glm::vec3& position);
		void updatePlayerLookingAt(const glm::ivec3& lookingAt, const Cube::Face& face);
		void updateChunkNumbers(const int visible, const int active, const int total);
		void updateMousePosition();
	};
}

#endif