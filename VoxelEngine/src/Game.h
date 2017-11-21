#ifndef GAME_H
#define GAME_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>

namespace Voxel
{
	// Foward
	class Program;
	class ChunkMap;
	class ChunkMeshGenerator;
	class InputHandler;
	class Player;
	class DebugConsole;
	class ChunkWorkManager;
	class Skybox;
	class Calendar;
	class World;
	class Physics;
	class Setting;
	class WorldMap;

	namespace UI
	{
		class Canvas;
		class Text;
		class Cursor;
	}

	/**
	*	@class Game
	*	@brief A game. Manages everything that is in the game (World, player, input, etc)
	*/
	class Game
	{
	private:
		enum class UI_Z_ORDER
		{
			CROSS_HAIR = 0,
			TIME,
		};
	public:
		enum class CursorState
		{
			HIDDEN = 0,
			SHOWN,
		};

		enum class LoadingState
		{
			INITIALIZING = 0,
			FADING,
			RELOADING,
			FINISHED,
		};

		enum class ReloadState
		{
			NONE = 0,
			CHUNK_MAP,		// Reload chunk map
			CHUNK_MESH,		// Reload chunk meshes only
			WORLD,			// Reload world
		};

		enum class GameState
		{
			IDLE = 0,		// Renders world
			VIEWING_WORLD_MAP,		// Renders world map
		};
	private:
		// Camera modes
		bool cameraMode;
		bool cameraControlMode;
		
		// For camera movement
		glm::vec3 getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance);

		// States
		CursorState cursorState;
		LoadingState loadingState;
		ReloadState reloadState;
		GameState gameState;
		
		// global seed
		std::string globalSeed;

		// World
		World* world;

		// Chunks
		ChunkMap* chunkMap;
		ChunkMeshGenerator* chunkMeshGenerator;
		ChunkWorkManager* chunkWorkManager;

		// physics
		Physics* physics;

		// Player
		Player* player;

		// skybox
		Skybox* skybox;

		// Calendar
		Calendar* calendar;
		
		// Setting instance ptr
		Setting* settingPtr;

		// World map
		WorldMap* worldMap;

		// UI canvases
		UI::Canvas* defaultCanvas;
		UI::Canvas* loadingCanvas;

		// cursor
		UI::Cursor* cursor;

		// Mouse pos. InputHandler only keep tracks mouse pos when it moves
		glm::vec2 prevMouseCursorPos;

		// Set this to true to skip update. Input update still works because some inputs needs to skip update.
		bool skipUpdate;

		// Initialize/release instances
		void init();
		void release();

		// Initialize sprite sheets
		void initSpriteSheets();

		// Mesh builder thread
		void initMeshBuilderThread();

		// Create/Load/unload player
		void createPlayer();

		// Create/Load/release chunkMap
		void createChunkMap();

		// Create world
		void createWorld();

		// Initialize everything that uses random
		void initRandoms();

		// Init UI 
		void initUI(); 
		void initLoadingScreen();
		void initDefaultCanvas();
		void initDebugConsole();
		void initCursor();
		void initSkyBox();
		void initWorldMap();

		// Update inputs
		void updateKeyboardInput(const float delta);
		void updateMouseMoveInput(const float delta);
		void updateMouseClickInput();
		void updateMouseScrollInput(const float delta);
		void updateControllerInput(const float delta);

		void updatePhysics(const float delta);

		// Check if chunk loader needs to be updated
		void updateChunks();
		void updatePlayerRaycast();
		void updatePlayerCameraCollision();

		void openWorldMap();
		void closeWorldMap();

		// Replace player to highest y 
		void replacePlayerToTopY();
		
		// Check if there is any chunks that needs to be unloaded
		void checkUnloadedChunks();

		void toggleCursorMode(const bool mode);
	public:
		Game();
		~Game();

		// create new. 
		void createNew(const std::string& worldName);
		// load from existing save
		void load(const std::string& worldPath);

		World* getWorld();

		// input handler instance
		InputHandler* input;

		// debug
		DebugConsole* debugConsole;

		// teleport player
		void teleportPlayer(const glm::vec3& position);

		// Updates (tick) the world.
		void update(const float delta);

		// refresh chunk map. Release all meshes and rebuild mesh only.
		void refreshChunkMap();
		// wipe out entire chunk map and re construct the chunk map
		void rebuildChunkMap();

		// Rebuilds world. It also rebuilds chunk map
		void rebuildWorld();

		void render(const float delta);
		void renderGame(const float delta);
		void renderWorld(const float delta);
		void renderWorldMap(const float delta);
		void renderLoadingScreen(const float delta);
		void renderUI();
		void renderDebugConsole();

		// debug console function
		void setFogEnabled(const bool enabled);
	};
}

#endif