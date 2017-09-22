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
	class World;

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
		//static const int UNLOAD_WORK = 1;
		static const int PRE_GENERATE_WORK = 2;
		static const int GENERATE_WORK = 3;
		static const int BUILD_MESH_WORK = 4;
		static const int SMOOTH_WORK = 5;
	private:
		// Queue with chunk coordinate that nees to pre generate chunk datas(height map, region map, etc)
		std::list<glm::ivec2> preGenerateQueue;
		std::list<glm::ivec2> smoothQueue;
		// Queue with chunk coordinate that needs to generate chunk
		std::list<glm::ivec2> generateQueue;
		// Queue with chunk coordinates that need to build mesh
		std::list<glm::ivec2> buildMeshQueue;
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
		void work(ChunkMap* map, ChunkMeshGenerator* meshGenerator, World* world);
	public:
		ChunkWorkManager();
		~ChunkWorkManager() = default;

		// Add single load work to load queue. locked by queueMutex
		void addPreGenerateWork(const glm::ivec2& coordinate, const bool highPriority = false);
		// Add multiple load works to load queue. locked by queueMutex
		void addPreGenerateWorks(const std::vector<glm::ivec2>& coordinates, const bool highPriority = false);

		void addSmoothWork(const glm::ivec2& coordinate, const bool highPriority = false);

		// Add single load work to generate queue. locked by queueMutex
		void addGenerateWork(const glm::ivec2& coordinate, const bool highPriority = false);
		// Add multiple load works to generate queue. locked by queueMutex
		void addGenerateWorks(const std::vector<glm::ivec2>& coordinates, const bool highPriority = false);

		// Add single build mesh work to build mesh queue. locked by queueMutex
		void addBuildMeshWork(const glm::ivec2& coordinate, const bool highPriority = false);
		// Add mutliple build mesh works to unload queue. locked by queueMutex
		void addBuildMeshWorks(const std::vector<glm::ivec2>& coordinates, const bool highPriority = false);

		// sort load queue based on chunk position that player is on. Locked by queueMutex
		void sortLoadQueue(const glm::vec3& playerPosition);

		// Add unload work to finished queue to let main thread know. Locked by finishedQueueMutex
		void addFinishedQueue(const glm::ivec2& coordinate);
		// Get first in finished queue. Only used by mainthread. Locked by finishedQueueMutex
		bool getFinishedFront(glm::ivec2& coordinate);
		// Pop the first in finished queue. Only used by mainthread. Locked by finishedQueueMutex
		void popFinishedAndNotify();

		// Creates the thread. Make sure you call once after run.
		void createThreads(ChunkMap* map, ChunkMeshGenerator* meshGenerator, World* world, const int coreCount);

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