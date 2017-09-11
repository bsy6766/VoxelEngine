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
		static const int IDLE_WORK = 0;
		static const int UNLOAD_WORK = 1;
		static const int LOAD_WORK = 2;
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
		std::vector<std::thread> workerThreads;
		//std::thread meshBuilderThread;

		// True if work manager is running. Else, false. 
		std::atomic<bool> running;
		
		// Condition variable that makes thread to wait if main thread is busy or there is no work to do
		std::condition_variable cv;

		// For mesh build thread
		void processChunk(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator);
	public:
		ChunkWorkManager();
		~ChunkWorkManager() = default;

		// Add single load work to load queue. locked by queueMutex
		void addLoad(const glm::ivec2& coordinate, const bool highPriority = false);
		// Add multiple load works to load queue. locked by queueMutex
		void addLoad(const std::vector<glm::ivec2>& coordinates, const bool highPriority = false);
		// Add single unload work to unload queue. locked by queueMutex
		void addUnload(const glm::ivec2& coordinate);
		// Add mutliple unload works to unload queue. locked by queueMutex
		void addUnload(const std::vector<glm::ivec2>& coordinates);
		// sort load queue based on chunk position that player is on. Locked by queueMutex
		void sortLoadQueue(const glm::vec3& playerPosition);
		// Add unload work to finished queue to let main thread know. Locked by finishedQueueMutex
		void addFinishedQueue(const glm::ivec2& coordinate);
		// Get first in finished queue. Only used by mainthread. Locked by finishedQueueMutex
		bool getFinishedFront(glm::ivec2& coordinate);
		// Pop the first in finished queue. Only used by mainthread. Locked by finishedQueueMutex
		void popFinishedAndNotify();

		// Creates the thread. Make sure you call once after run.
		void createThreads(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator, const int coreCount);

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