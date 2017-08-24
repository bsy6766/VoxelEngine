#ifndef CHUNK_MESH_MANAGER
#define CHUNK_MESH_MANAGER

#include <list>
#include <glm\glm.hpp>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>

namespace Voxel
{
	class ChunkMap;
	class ChunkMeshGenerator;

	/**
	*	@class Chunk Mesh Manager.
	*	@brief Manages chunk mesh generation. 
	*	
	*	This class manages chunks that need a mesh to be generated.
	*	Main thread will add chunk coordinate that needs mesh,
	*	then mesh bulder thread will grab one (FIFO) coordinate
	*	and generate mesh.
	*	When it's finish, thread will mark chunk as loaded. 
	*/
	class ChunkWorkManager
	{
	private:
		std::list<glm::ivec2> chunkQueue;

		std::mutex queueMutex;

		std::thread meshBuilderThread;

		std::atomic<bool> running;
		std::condition_variable cv;

		// For mesh build thread
		void buildMesh(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator);
	public:
		ChunkWorkManager();
		~ChunkWorkManager();

		// Main thread adds coordinate
		void addChunkCoordinate(const glm::ivec2& coordinate);

		// Creates the thread. Make sure you call once after run.
		void createThread(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator);

		// Start running mesh builder
		void run();
		// Stop mesh building.
		void stop();
		// Join threads
		void joinThread();
	};
}

#endif