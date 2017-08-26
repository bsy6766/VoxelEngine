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
		// Queue with chunk coordinate that needs to either generate or build mesh. Can be both
		std::list<glm::ivec2> loadQueue;
		// Queue with chunk coordinate that needs to get unloaded.
		std::list<glm::ivec2> unloadQueue;
		// Queue with chunk coordinate that needs to get unloaded by main thread.
		std::list<glm::ivec2> unloadFinishedQueue;

		// Mutex for loadQueue and unloadQueue.
		std::mutex queueMutex;
		// Mutex for unloadFinishedQueue.
		std::mutex finishedQueueMutex;

		// Worker thread.
		std::thread meshBuilderThread;

		// True if work manager is running. Else, false. 
		std::atomic<bool> running;
		
		// Condition variable that makes thread to wait if main thread is busy or there is no work to do
		std::condition_variable cv;

		// For mesh build thread
		void processChunk(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator);
	public:
		ChunkWorkManager();
		~ChunkWorkManager();

		// Add or remove work
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