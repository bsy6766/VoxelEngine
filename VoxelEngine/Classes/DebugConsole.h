#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H

// voxel
#include "Config.h"

#if V_DEBUG && V_DEBUG_CONSOLE

// gl
#include <GL\glew.h>
#include <GLFW\glfw3.h>

// cpp
#include <string>
#include <list>

// glm
#include <glm\glm.hpp>

// voxel
#include "Cube.h"
#include "UI.h"

namespace Voxel
{
	class Player;
	class Game;
	class ChunkMap;
	class World;
	class Setting;
	class Calendar;

	class DebugConsole
	{
	private:
		bool openingConsole;
		bool debugOutputVisibility;

		unsigned int lastCommandIndex;

		const std::string DefaultCommandInputText = "ENTER COMMAND";

		UI::Canvas* debugCanvas;

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

		UI::Text* drawCallAndVertCount;

		UI::Text* biomeAndTerrainInfo;
		UI::Text* regionID;

		UI::Text* worldMapRegionID;
		UI::Text* worldMapRegionSitePos;
		
		UI::Image* commandInputField;
		UI::Image* commandHistoryBg;
		UI::Text* command;
		UI::Text* commandHistorys;

		std::list<std::string> executedCommandHistory;
		std::list<std::string> lastCommands;

		bool executeCommand(const std::string& command);
		void addCommandHistory(const std::string& command);
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
		Calendar* calendar;

		void init();

		void openConsole();
		void closeConsole();

		bool isConsoleOpened();

		void updateConsoleInputText(const std::string& c);

		void render();

		void toggleDubugOutputs();

		void onFPSUpdate(int fps);
		
		void update(const float delta);
		void updateMouseMove(const glm::vec2& mousePosition);
		void updateMouseClick(const glm::vec2& mousePosition, const int button);
		void updateMouseRelease(const glm::vec2& mousePosition, const int button);
		void updateResolution(int width, int height);
		void updateVsync(bool vsync);
		void updatePlayerPosition(const glm::vec3& position);
		void updatePlayerRotation(const glm::vec3& rotation);
		void updatePlayerLookingAt(const glm::ivec3& lookingAt, const Cube::Face& face);
		void setPlayerLookingAtVisibility(const bool visibility);
		void updateChunkNumbers(const int visible, const int active, const int total, const std::string& workOrder);
		void updateBiome(const std::string& biomeType, const std::string& terrainType, const float t, const float m);
		void updateRegion(const unsigned int regionID);
		
		//void updateDrawCallsAndVerticesSize();

#if V_DEBUG && V_DEBUG_UI_TEST
		Voxel::UI::Image* testImage;
		Voxel::UI::AnimatedImage* testAnimatedImage;
		Voxel::UI::Text* testText;
		Voxel::UI::Button* testButton;
		Voxel::UI::Button* testDisabledButton;
		Voxel::UI::CheckBox* testCheckBox;
		Voxel::UI::CheckBox* testDisabledCheckBox;
		Voxel::UI::ProgressTimer* testProgHorBar0;
		Voxel::UI::ProgressTimer* testProgHorBar50;
		Voxel::UI::ProgressTimer* testProgHorBar100;
		Voxel::UI::ProgressTimer* testProgHorBar0ccw;
		Voxel::UI::ProgressTimer* testProgHorBar50ccw;
		Voxel::UI::ProgressTimer* testProgHorBar100ccw;
		Voxel::UI::ProgressTimer* testProgVerBar0;
		Voxel::UI::ProgressTimer* testProgVerBar50;
		Voxel::UI::ProgressTimer* testProgVerBar100;
		Voxel::UI::ProgressTimer* testProgVerBar0ccw;
		Voxel::UI::ProgressTimer* testProgVerBar50ccw;
		Voxel::UI::ProgressTimer* testProgVerBar100ccw;
		Voxel::UI::ProgressTimer* testProgRadial0;
		Voxel::UI::ProgressTimer* testProgRadial25;
		Voxel::UI::ProgressTimer* testProgRadial50;
		Voxel::UI::ProgressTimer* testProgRadial75;
		Voxel::UI::ProgressTimer* testProgRadial100;
		Voxel::UI::ProgressTimer* testProgRadial0ccw;
#endif
	};
}

#endif

#endif