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
		class Text2;
		class Image;
	}

	class Player;
	class Game;
	class ChunkMap;
	class World;
	class Setting;

	class DebugConsole
	{
	private:
		bool openingConsole;
		bool debugOutputVisibility;

		const std::string DefaultCommandInputText = "ENTER COMMAND";

		UI::Canvas* debugCanvas;

		//UI::Text* staticLabels;
		UI::Text2* fpsNumber;
		UI::Text2* resolutionNumber;
		UI::Text2* vsyncMode;

		UI::Text2* hardwardInfo;

		UI::Text2* mousePosition;
		UI::Text2* cameraPosition;
		UI::Text2* cameraRotation;

		UI::Text2* playerPosition;
		UI::Text2* playerRotation;
		UI::Text2* playerChunkPosition;
		UI::Text2* playerLookingAt;

		UI::Text2* chunkNumbers;

		UI::Text2* biome;
		UI::Text2* region;

		UI::Image* commandInputField;
		UI::Image* commandHistoryBg;
		UI::Text2* command;
		UI::Text2* commandHistorys;

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
		Setting* settingPtr;

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
		void updateChunkNumbers(const int visible, const int active, const int total, const std::string& workOrder);
		void updateBiome(const std::string& biomeType, const std::string& terrainType, const float t, const float m);
		void updateRegion(const unsigned int regionID);
	};
}

#endif