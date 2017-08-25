#ifndef CHUNK_MESH_MANAGER
#define CHUNK_MESH_MANAGER

#include <list>
#include <glm\glm.hpp>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <vector>

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
		//std::list<WorkTicket> loadWorkQueue;
		std::list<glm::ivec2> loadQueue;
		std::list<glm::ivec2> unloadQueue;
		std::list<glm::ivec2> unloadFinishedQueue;

		std::mutex queueMutex;
		std::mutex finishedQueueMutex;

		std::thread meshBuilderThread;

		std::atomic<bool> running;
		std::condition_variable cv;

		// For mesh build thread
		void processChunk(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator);
	public:
		ChunkWorkManager();
		~ChunkWorkManager();

		// Main thread adds coordinate
		void addLoad(const glm::ivec2& coordinate);
		void addLoad(const std::vector<glm::ivec2>& coordinates);
		void addUnload(const glm::ivec2& coordinate);
		void addUnload(const std::vector<glm::ivec2>& coordinates);

		void addFinishedQueue(const glm::ivec2& coordinate);
		bool getFinishedFront(glm::ivec2& coordinate);
		void popFinishedAndNotify();

		// Creates the thread. Make sure you call once after run.
		void createThread(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator);

		// notify condition variable
		void notify();

		// Start running mesh builder
		void run();
		// Stop mesh building.
		void stop();
		// Join threads
		void joinThread();
	};
}

#endif