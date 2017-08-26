#ifndef WORLD_H
#define WORLD_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>

namespace Voxel
{
	// Foward
	class Program;
	class ChunkMap;
	class ChunkLoader;
	class ChunkMeshGenerator;
	class InputHandler;
	class Player;
	class DebugConsole;
	class ChunkWorkManager;
	class Skybox;

	namespace UI
	{
		class Canvas;
		class Text;
	}

	/**
	*	@class World
	*	@brief A world where everything is in. Terrain, player, weather, skybox, entity, items and all the stuffs that are rendered or even those doesen't.
	*
	*	For now, World instance is managed by Application class.
	*	There will be only one world at a time. There is no plan for zone phase shift or whatnot.
	*	Just giant single world where (theoretically) infinitely generates.
	*/
	class World
	{
	private:
		// Store default program
		Program* defaultProgram;
		
		// For looking at chube
		GLuint cvao;
		GLuint cvbo;
		GLuint ccbo;
		GLuint cibo;

		bool cameraMode;
		bool keyCDown;
		bool cameraControlMode;
		bool keyXDown;
		glm::vec3 getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance);

		// Chunks
		ChunkMap* chunkMap;
		ChunkLoader* chunkLoader;
		ChunkMeshGenerator* chunkMeshGenerator;
		ChunkWorkManager* chunkWorkManager;

		// Player
		Player* player;

		// skybox
		Skybox* skybox;

		// Default UI canvas
		UI::Canvas* defaultCanvas;

		// Mouse pos. InputHandler only keep tracks mouse pos when it moves
		double mouseX;
		double mouseY;

		// Initialize/release instances
		void init();
		void release();

		// Mesh builder thread
		void initMeshBuilderThread();

		// Create/Load/unload player
		void createPlayer();
		//void loadPlayer();
		//void unload();

		// Create/Load/release chunkMap
		void createChunkMap();

		// Load chunkMap
		void loadChunkLoader();

		// Load chunk meshes
		void loadChunkMesh();

		void initUI();
		void initSkyBox(const glm::vec4& skyColor);

		void updateKeyboardInput(const float delta);
		void updateMouseMoveInput(const float delta);
		void updateMouseClickInput();
		void updateControllerInput(const float delta);
		// Check if chunk loader needs to be updated
		void updateChunks();

		void checkUnloadedChunks();
	public:
		World();
		~World();

		// create new. 
		void createNew(const std::string& worldName);
		// load from existing save
		void load(const std::string& worldPath);

		// input handler instance
		InputHandler* input;

		// debug
		DebugConsole* debugConsole;

		// Updates (tick) the world.
		void update(const float delta);
		void render(const float delta);
	};
}

#endif