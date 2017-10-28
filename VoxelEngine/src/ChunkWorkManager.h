#ifndef CHUNK_MESH_MANAGER
#define CHUNK_MESH_MANAGER

#include <list>
#include <glm\glm.hpp>
#include <ChunkUtil.h>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <vector>
#include <unordered_set>
#include <string>

namespace Voxel
{
	class ChunkMap;
	class ChunkMeshGenerator;
	class World;

	/**
	*	@class Chunk Mesh Manager.
	*	@brief Manages chunk mesh generation. 
	*	
	*	Steps
	*	1) PreGenerate chunk
	*	- Find which region the chunk is at.
	*	- Generate height map
	*	2) Smooth 
	*	- If chunk is in multiple regions, interplate the height map based on nearby chunk's height
	*	3) Generate
	*	- Initialize chunk sections
	*	- Add blocks based on height map
	*	
	*/
	class ChunkWorkManager
	{
	private:
		enum class WorkType
		{
			IDLE = 0,
			UNLOAD,
			PRE_GENERATE,
			SMOOTH,
			GENERATE,
			ADD_STRUCTURE,
			BUILD_MESH
		};

		enum class WORK_STATE
		{
			IDLE = 0,
			RUNNING,						// running. do work.
			CLEARING,						// clearing work. Releasing all mesh data and add work to unloadFinishedQueue.
			WAITING_MAIN_THREAD,			// Waits for main thread to change state.
		};
	private:
		// Queue with chunk coordinate that nees to pre generate chunk datas(height map, region map, etc)
		std::list<glm::ivec2> preGenerateQueue;
		// Queue with chunk coordinate that needs to smooth height map
		std::list<glm::ivec2> smoothQueue;
		// Queue with chunk coordinate that needs to generate chunk
		std::list<glm::ivec2> generateQueue;
		// Queue with chunk coordinate that needs to add structure to chunk
		std::list<glm::ivec2> addStructureQueue;
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

		// True if first initialization is done (ignores build mesh)
		std::atomic<bool> firstInitDone;

		// work state
		std::atomic<WORK_STATE> workState;
		
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
		void addStructureWork(const glm::ivec2& coordinate, const bool highPriority = false);

		// Add single load work to generate queue. locked by queueMutex
		void addGenerateWork(const glm::ivec2& coordinate, const bool highPriority = false);
		// Add multiple load works to generate queue. locked by queueMutex
		void addGenerateWorks(const std::vector<glm::ivec2>& coordinates, const bool highPriority = false);

		// Add single build mesh work to build mesh queue. locked by queueMutex
		void addBuildMeshWork(const glm::ivec2& coordinate, const bool highPriority = false);
		// Add mutliple build mesh works to unload queue. locked by queueMutex
		void addBuildMeshWorks(const std::vector<glm::ivec2>& coordinates, const bool highPriority = false);

		// sort load queue based on chunk position that player is on. Locked by queueMutex
		//void sortBuildMeshQueue(const glm::vec3& playerPosition);
		void sortBuildMeshQueue(const glm::ivec2& currentChunkXZ);
		void sortBuildMeshQueue(const glm::ivec2& currentChunkXZ, const std::vector<glm::ivec2>& visibleChunks);
		void sortBuildMeshQueue(const glm::ivec2& currentChunkXZ, const std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs>& visibleChunks);

		// Add unload work to finished queue to let main thread know. Locked by finishedQueueMutex
		void addFinishedQueue(const glm::ivec2& coordinate);
		// Get first in finished queue. Only used by mainthread. Locked by finishedQueueMutex
		bool getUnloadFinishedQueueFront(glm::ivec2& coordinate);
		// Pop the first in finished queue. Only used by mainthread. Locked by finishedQueueMutex
		void popFinishedAndNotify();

		// Creates the thread. Make sure you call once after run.
		void createThreads(ChunkMap* map, ChunkMeshGenerator* meshGenerator, World* world, const int coreCount);

		bool isFirstInitDone();

		// Clear all work order
		void clear();

		// Check if work manager is aboring all work
		bool isClearing();

		// Check if work manager is waiting for main thread
		bool isWaitingMainThread();

		// Resume work
		void resumeWork();

		// Check if chunk work manager is generating chunks yet
		bool isGeneratingChunks();

		// notify condition variable
		void notify();

		// Start running mesh builder
		void run();
		// Stop mesh building.
		void stop();
		// Join threads
		void joinThread();

		// for debug
		std::string getDebugOutput();
	};
}

#endif