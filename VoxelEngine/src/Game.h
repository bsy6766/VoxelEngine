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
		// Camera modes
		bool cameraMode;
		bool cameraControlMode;

		// For camera movement
		glm::vec3 getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance);
		
		// World
		World* world;

		// Chunks
		ChunkMap* chunkMap;
		ChunkMeshGenerator* chunkMeshGenerator;
		ChunkWorkManager* chunkWorkManager;

		// Player
		Player* player;

		// skybox
		Skybox* skybox;

		// Calendar
		Calendar* calendar;

		// Default UI canvas
		UI::Canvas* defaultCanvas;

		// cursor
		UI::Cursor* cursor;

		// Mouse pos. InputHandler only keep tracks mouse pos when it moves
		double mouseX;
		double mouseY;

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
		void initSkyBox(const glm::vec4& skyColor, Program* program);

		// Update inputs
		void updateKeyboardInput(const float delta);
		void updateMouseMoveInput(const float delta);
		void updateMouseClickInput();
		void updateControllerInput(const float delta);

		// Check if chunk loader needs to be updated
		void updateChunks();
		void updatePlayerRaycast();
		
		// Check if there is any chunks that needs to be unloaded
		void checkUnloadedChunks();
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

		// Updates (tick) the world.
		void update(const float delta);
		void updateInput(const float delta);

		void render(const float delta);

		// debug console function
		void setFogEnabled(const bool enabled);
	};
}

#endif