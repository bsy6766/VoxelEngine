#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <list>
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
	class Game;
	class ChunkMap;
	class World;

	class DebugConsole
	{
	private:
		bool openingConsole;
		bool debugOutputVisibility;

		const std::string DefaultCommandInputText = "ENTER COMMAND";

		UI::Canvas* debugCanvas;

		//UI::Text* staticLabels;
		UI::Text* fpsNumber;
		UI::Text* resolutionNumber;
		UI::Text* vsyncMode;

		UI::Text* hardwardInfo;

		UI::Text* mousePosition;
		UI::Text* cameraPosition;
		UI::Text* cameraRotation;

		UI::Text* playerPosition;
		UI::Text* playerRotation;
		UI::Text* playerChunkPosition;
		UI::Text* playerLookingAt;

		UI::Text* chunkNumbers;

		UI::Text* biome;
		UI::Text* region;

		UI::Image* commandInputField;
		UI::Image* commandHistoryBg;
		UI::Text* command;
		UI::Text* commandHistorys;

		std::list<std::string> executedCommandHistory;
		std::string lastCommand;

		bool executeCommand(const std::string& command);
		void updateCommandHistory();
	public:
		DebugConsole();
		~DebugConsole();

		// instances for debug
		Player* player;
		Game* game;
		World* world;
		ChunkMap* chunkMap;
		 
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
		void updatePlayerRotation(const glm::vec3& rotation);
		void updatePlayerLookingAt(const glm::ivec3& lookingAt, const Cube::Face& face);
		void setPlayerLookingAtVisibility(const bool visibility);
		void updateChunkNumbers(const int visible, const int active, const int total);
		void updateBiome(const std::string& biomeType, const std::string& terrainType, const float t, const float m);
		void updateRegion(const unsigned int regionID);
	};
}

#endif