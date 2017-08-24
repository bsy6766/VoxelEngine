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

	/*
	struct WorkTicket
	{
	public:
		enum class TYPE : unsigned int
		{
			LOAD = 0,
			UNLOAD
		};

		TYPE type;
		glm::ivec2 chunkCoordinate;

		WorkTicket(const glm::ivec2 chunkCoordinate, const TYPE type)
			: chunkCoordinate(chunkCoordinate)
			, type(type)
		{}
	};
	*/

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
		void buildMesh(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator);
	public:
		ChunkWorkManager();
		~ChunkWorkManager();

		// Main thread adds coordinate
		//void addWorkTicket(const glm::ivec2& coordinate, const WorkTicket::TYPE type);
		void addLoad(const glm::ivec2& coordinate);
		void addUnload(const glm::ivec2& coordinate);

		void addFinishedQueue(const glm::ivec2& coordinate);
		bool popFinishedQueue(glm::ivec2& coordinate);

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