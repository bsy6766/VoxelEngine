#include "ChunkWorkManager.h"
#include <iostream>
#include <ChunkMap.h>
#include <Chunk.h>
#include <ChunkUtil.h>
#include <ChunkSection.h>
#include <ChunkMeshGenerator.h>
#include <ChunkMesh.h>
#include <Utility.h>

using namespace Voxel;

ChunkWorkManager::ChunkWorkManager()
{
}

ChunkWorkManager::~ChunkWorkManager()
{
}

void Voxel::ChunkWorkManager::addLoad(const glm::ivec2 & coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);
	bool wasEmpty = loadQueue.empty();
	loadQueue.push_back(coordinate);
	cv.notify_one();
}

void Voxel::ChunkWorkManager::addUnload(const glm::ivec2 & coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);
	bool wasEmpty = unloadQueue.empty();
	unloadQueue.push_back(coordinate);
	cv.notify_one();

	loadQueue.remove(coordinate);
}

void Voxel::ChunkWorkManager::addFinishedQueue(const glm::ivec2 & coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(finishedQueueMutex);

	unloadFinishedQueue.push_back(coordinate);
}

bool Voxel::ChunkWorkManager::popFinishedQueue(glm::ivec2& coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(finishedQueueMutex);

	if (!unloadFinishedQueue.empty())
	{
		coordinate = unloadFinishedQueue.front();
		unloadFinishedQueue.pop_front();
		std::cout << "Main thread has (" << coordinate.x << ", " << coordinate.y << ") to unload" << std::endl;
		return true;
	}

	return false;
}

/*
void Voxel::ChunkWorkManager::addWorkTicket(const glm::ivec2 & coordinate, const WorkTicket::TYPE type)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);
	// Add new ticket to back
	loadWorkQueue.push_back(WorkTicket(coordinate, type));
	// notify thread
	cv.notify_one();
	//std::cout << "Main thread added (" << coordinate.x << ", " << coordinate.y << ")" << std::endl;
}
*/

void Voxel::ChunkWorkManager::buildMesh(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator)
{
	// loop while it's running
	//std::cout << "Thraed #" << std::this_thread::get_id() << " started to build mesh " << std::endl;
	while (running)
	{
		{
			// Scope lock
			std::unique_lock<std::mutex> lock(queueMutex);

			// wait if both queue are empty. Must be running.
			while (running && loadQueue.empty() && unloadQueue.empty())
			{
				cv.wait(lock);
			}

			if (running == false)
			{
				// quit
				break;
			}


			if (map && chunkMeshGenerator)
			{
				// Process unload queue first.
				if (!unloadQueue.empty())
				{
					// There is chunk to unload.
					auto chunkXZ = unloadQueue.front();
					unloadQueue.pop_front();

					std::cout << "Thraed #" << std::this_thread::get_id() << " has (" << chunkXZ.x << ", " << chunkXZ.y << "): Unload" << std::endl;

					bool hasChunk = map->hasChunkAtXZ(chunkXZ.x, chunkXZ.y);
					if (hasChunk)
					{
						auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
						if (chunk)
						{
							auto mesh = chunk->getMesh();
							if (mesh)
							{
								// Clear buffer. 
								mesh->clearBuffers();
								// Let main thread to relase it
								addFinishedQueue(chunkXZ);
							}
							// Else, mesh is nullptr
						}
						// Else, chunk is nullptr
					}
					// Else, has no chunk
				}
				else
				{
					// There is no chunk to unload. 
					if (!loadQueue.empty())
					{
						// There is chunk to load
						auto chunkXZ = loadQueue.front();
						loadQueue.pop_front();
						std::cout << "Thraed #" << std::this_thread::get_id() << " has (" << chunkXZ.x << ", " << chunkXZ.y << "): Load" << std::endl;

						// There must be a chunk. Chunk loader creates empty chunk.
						bool hasChunk = map->hasChunkAtXZ(chunkXZ.x, chunkXZ.y);
						if (hasChunk)
						{
							auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
							if (chunk)
							{
								auto mesh = chunk->getMesh();
								if (mesh)
								{
									if (!mesh->hasBufferToLoad())
									{
										// Mesh doesn't have buffer to load
										chunkMeshGenerator->generateSingleChunkMesh(chunk, map);
									}
								}
								// Else, mesh is nullptr
							}
							// Else, chunk is nullptr
						}
						// Else, has no chunk
					}
				}
			}
			else
			{
				throw std::runtime_error("Map or chunk mesh generator is nullptr.");
			}
			

			//std::cout << "Thraed #" << std::this_thread::get_id() << " has (" << ticket.chunkCoordinate.x << ", " << ticket.chunkCoordinate.y << "), Type: " << static_cast<int>(ticket.type) << std::endl;

			//auto start = Utility::Time::now();
			// For now, assume chunk is all loaded.

			//auto end = Utility::Time::now();
			//std::cout << "Elapsed time: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
		}
	}
}

void Voxel::ChunkWorkManager::createThread(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator)
{
	if (running)
	{
		meshBuilderThread = std::thread(&ChunkWorkManager::buildMesh, this, map, chunkMeshGenerator);
	}
}

void Voxel::ChunkWorkManager::run()
{
	running.store(true);
}

void Voxel::ChunkWorkManager::stop()
{
	running.store(false);
	cv.notify_all();
}

void Voxel::ChunkWorkManager::joinThread()
{
	{
		//std::cout << "Waiting to thread join..." << std::endl;
		// Scope lock
		std::unique_lock<std::mutex> lock(queueMutex);
		if (meshBuilderThread.joinable())
		{
			std::cout << "joining thread #" << meshBuilderThread.get_id() << std::endl;
			meshBuilderThread.join();
		}
	}
}