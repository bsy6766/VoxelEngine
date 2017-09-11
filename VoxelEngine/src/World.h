#ifndef WORLD_H
#define WORLD_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>

// Temp
#include <Voronoi.h>

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
		
		// For looking at cube
		GLuint cvao;
		GLuint cvbo;
		GLuint ccbo;
		GLuint cibo;

		// Camera modes
		bool cameraMode;
		bool cameraControlMode;

		// For camera movement
		glm::vec3 getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance);

		// render modes
		bool renderChunks;
		bool renderVoronoi;

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

		// Initialize everything that uses random
		void initRandoms();

		// Init UI 
		void initUI();
		void initCubeOutline();
		void initSkyBox(const glm::vec4& skyColor);

		void updateKeyboardInput(const float delta);
		void updateMouseMoveInput(const float delta);
		void updateMouseClickInput();
		void updateControllerInput(const float delta);
		// Check if chunk loader needs to be updated
		void updateChunks();
		void updatePlayerRaycast();

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

		//temp
		Voronoi::Diagram* vd;
		void initVoronoi();

		// Updates (tick) the world.
		void update(const float delta);
		void updateInput(const float delta);

		void render(const float delta);


		// debug console function
		void setRenderChunkMode(const bool mode);
		void setRenderVoronoiMode(const bool mode);
	};
}

#endif