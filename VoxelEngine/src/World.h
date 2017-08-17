#ifndef WORLD_H
#define WORLD_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

// Multithreading
#include <thread>
#include <mutex>
#include <atomic>
#include <list>
#include <condition_variable>

//temp
#include <ChunkMesh.h>

namespace Voxel
{
	// Foward
	class Program;
	class ChunkMap;
	class ChunkLoader;
	class ChunkMeshGenerator;
	class InputHandler;
	class Player;

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

		/*
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
		glm::vec3 angle = glm::vec3(0);
		glm::mat4 tempRotation = glm::mat4(1.0f);
		glm::mat4 tempTralsnate = glm::mat4(1.0f);
		void initDebugCube();
		ChunkMesh* chunkMesh;
		*/
		/*
		ChunkMesh* debugPlayerCube;
		void initDebugPlayerCube();
		GLuint vao;
		GLuint vbo;
		GLuint cbo;
		*/

		// For camera frustum
		GLuint vao;
		GLuint vbo;
		GLuint cbo;
		void initDebugCamerafrustum();

		GLuint pvao;
		GLuint pvbo;
		GLuint rvao;
		GLuint rvbo;

		bool cameraMode;
		bool keyCDown;
		bool cameraControlMode;
		bool keyXDown;
		glm::vec3 getMovedDistByKeyInput(const float angleMod, const glm::vec3 axis, float distance);

		// Chunks
		ChunkMap* chunkMap;
		ChunkLoader* chunkLoader;
		ChunkMeshGenerator* chunkMeshGenerator;

		// Player
		Player* player;

		// Mouse pos. InputHandler only keep tracks mouse pos when it moves
		double mouseX;
		double mouseY;

		void initChunk();
		void initPlayer();

		void updateKeyboardInput(const float delta);
		void updateMouseMoveInput(const float delta);
		void updateMouseClickInput();
		void updateControllerInput(const float delta);
		// Check if chunk loader needs to be updated
		void updateChunks();
	public:
		World();
		~World();

		InputHandler* input;


		// threads
		std::vector<std::thread> testThreads;
		void testThreadFunc();
		std::atomic<bool> threadRunning;
		std::list<int> chunkQueue;
		float chunkElapsedTime;
		std::mutex chunkQueueMutex;
		std::condition_variable cv;

		// Updates (tick) the world.
		void update(const float delta);
		void render(const float delta);
	};
}

#endif