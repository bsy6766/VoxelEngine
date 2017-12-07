#ifndef GAME_H
#define GAME_H

// cpp
#include <string>

// glm
#include <glm\glm.hpp>

// gl
#include <GL\glew.h>

// glfw
#include <GLFW\glfw3.h>

// Voxel
#include <Config.h>

namespace Voxel
{
	// Foward declaration
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
		enum class UI_Z_ORDER : int
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

		// input handler instance
		InputHandler* input;

		// UIs
		UI::Canvas* staticCanvas;		// UI that is static. Usually rendered behind dynamic canvas
		UI::Canvas* dynamicCanvas;		// Canvas with ui that is dynamic.
		UI::Text* timeLabel;

		// Loading screen
		UI::Canvas* loadingCanvas;

		// cursor
		UI::Cursor* cursor;

		// Mouse pos. InputHandler only keep tracks mouse pos when it moves
		glm::vec2 prevMouseCursorPos;

		// Set this to true to skip update. Input update still works because some inputs needs to skip update.
		bool skipUpdate;

		// Initialize all sub system and instances
		void init();

		// Releasea ll sub system and instances.
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

		// Intialize loading screen
		void initLoadingScreen();

		// Initialize default UI canvas
		void initDefaultCanvas();

		// Initialize cursor
		void initCursor();

		// Initailize sky box and fog
		void initSkyBox();

		// Initialize world map
		void initWorldMap();

		/**
		*	Updates keyboard input. May interupt and skip update during debug mode.
		*	@param delta Elapsed time on current frame.
		*/
		void updateKeyboardInput(const float delta);

		/**
		*	Updates mouse movement input. Forces delta time to 60 fps
		*	@param delta Elapsed time on current frame.
		*/
		void updateMouseMoveInput(const float delta);

		// Updates mouse click input
		void updateMouseClickInput();

		/**
		*	Updates mouse scroll input.
		*	@param delta Elapsed time on current frame.
		*/
		void updateMouseScrollInput(const float delta);

		/**
		*	Updates controller input
		*	@param delta Elapsed time on current frame.
		*/
		void updateControllerInput(const float delta);

		/**
		*	Updates physics.
		*	Applies gravity and resolve collisions
		*	@param delta Elapsed time on current frame.
		*/
		void updatePhysics(const float delta);

		// Updates chunk map
		void updateChunks();

		// Updates player's raycast to world
		void updatePlayerRaycast();

		// Updates player's third person view camera collision
		void updatePlayerCameraCollision();

		// Opens world map
		void openWorldMap();

		// Closes world map
		void closeWorldMap();

		// Replace player to highest y 
		void replacePlayerToTopY();
		
		// Check if there is any chunks that needs to be unloaded
		void checkUnloadedChunks();
	public:
		// Constructor
		Game();

		// Destructor
		~Game();

		// create new. 
		void createNew(const std::string& worldName);
		
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

		/**
		*	Toggles cursor mode.
		*	If cursor is enabled, some other inputs won't work, such as player movement or rotation.
		*	@param mode Cursor mode to set. true to enable, false to disable.
		*/
		void toggleCursorMode(const bool mode);

		/**
		*	Set fog mode.
		*	@param enabled true to enable fog. false to disable fog.
		*/
		void setFogEnabled(const bool enabled);

		/**
		*	Root function for all rendering
		*	@param delta Elapsed time on current frame.
		*/
		void render(const float delta);

		/**
		*	Renders game based on game state
		*	@param delta Elapsed time on current frame.
		*/
		void renderGame(const float delta);

		/**
		*	Renders the world. Chunk map, player, etc.
		*	@param delta Elapsed time on current frame.
		*/
		void renderWorld(const float delta);

		/**
		*	Renders world map
		*	@param delta Elapsed time on current frame.
		*/
		void renderWorldMap(const float delta);

		/**
		*	Renders loading screen
		*	@param delta Elapsed time on current frame.
		*/
		void renderLoadingScreen(const float delta);

		/**
		*	Renders UI
		*	@param delta Elapsed time on current frame.
		*/
		void renderUI();

#if V_DEBUG
#if V_DEBUG_CONSOLE
		// Debug console
		DebugConsole* debugConsole;
		void initDebugConsole();
		void releaseDebugConsole();
		void renderDebugConsole();
#endif
#if V_DEBUG_CAMERA_MODE
		// Camera modes
		bool cameraMode;
		bool cameraControlMode;
#endif
#endif
	};
}

#endif