#include "ChunkWorkManager.h"
#include <iostream>
#include <ChunkMap.h>
#include <Chunk.h>
#include <ChunkUtil.h>
#include <ChunkSection.h>
#include <ChunkMeshGenerator.h>
#include <ChunkMesh.h>
#include <Utility.h>
#include <unordered_set>

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

	loadQueue.push_back(coordinate);
	cv.notify_one();
}

void Voxel::ChunkWorkManager::addLoad(const std::vector<glm::ivec2>& coordinates)
{
	auto start = Utility::Time::now();
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	for (auto xz : coordinates)
	{
		loadQueue.push_back(xz);
	}

	auto end = Utility::Time::now();
	std::cout << "addLoad() took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;

	//cv.notify_one();
}

void Voxel::ChunkWorkManager::addUnload(const glm::ivec2 & coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	unloadQueue.push_back(coordinate);
	loadQueue.remove(coordinate);

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addUnload(const std::vector<glm::ivec2>& coordinates)
{
	auto start = Utility::Time::now();
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs> lut(coordinates.begin(), coordinates.end());

	for (auto xz : coordinates)
	{
		unloadQueue.push_back(xz);
	}

	auto it = loadQueue.begin();
	for (; it != loadQueue.end();)
	{
		auto find_it = lut.find(*it);
		if (find_it != lut.end())
		{
			it = loadQueue.erase(it);
		}
		else
		{
			it++;
		}
	}

	auto end = Utility::Time::now();
	std::cout << "addUnload() took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;

	//cv.notify_one();
}

void Voxel::ChunkWorkManager::addFinishedQueue(const glm::ivec2 & coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(finishedQueueMutex);
	//std::cout << "Finished unloading (" << coordinate.x << ", " << coordinate.y << ")" << std::endl;
	unloadFinishedQueue.push_back(coordinate);
}

bool Voxel::ChunkWorkManager::getFinishedFront(glm::ivec2& coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(finishedQueueMutex);

	if (!unloadFinishedQueue.empty())
	{
		coordinate = unloadFinishedQueue.front();
		std::cout << "Main thread has (" << coordinate.x << ", " << coordinate.y << ") to unload" << std::endl;
		return true;
	}

	return false;
}

void Voxel::ChunkWorkManager::popFinishedAndNotify()
{
	// Scope lock
	std::unique_lock<std::mutex> lock(finishedQueueMutex);

	unloadFinishedQueue.pop_front();
	if (unloadFinishedQueue.empty())
	{
		cv.notify_one();
	}
}

void Voxel::ChunkWorkManager::processChunk(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator)
{
	// loop while it's running
	//std::cout << "Thraed #" << std::this_thread::get_id() << " started to build mesh " << std::endl;
	while (running)
	{
		glm::ivec2 chunkXZ;
		int flag = 0;

		{
			// Scope lock
			std::unique_lock<std::mutex> lock(queueMutex);

			// wait if both queue are empty. Must be running.
			while (running && loadQueue.empty() && unloadQueue.empty())
			{
				cv.wait(lock);
			}

			std::cout << "There is job to do!" << std::endl;

			if (running == false)
			{
				// quit
				break;
			}

			{
				// Scope lock
				std::unique_lock<std::mutex> fLock(finishedQueueMutex);
				while (!unloadFinishedQueue.empty())
				{
					cv.wait(fLock);
				}

				std::cout << "No need to wait!" << std::endl;
			}


			if (!unloadQueue.empty())
			{
				chunkXZ = unloadQueue.front();
				unloadQueue.pop_front();
				flag = 1;
			}
			else if (!loadQueue.empty())
			{
				chunkXZ = loadQueue.front();
				loadQueue.pop_front();
				flag = 2;
			}
		}


		if (map && chunkMeshGenerator)
		{
			if (flag == 1)
			{

				std::cout << "(" << chunkXZ.x << ", " << chunkXZ.y << "): Unload" << std::endl;

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
							//map->moveChunkToUnloadMap(chunkXZ);

							// Let main thread to relase it
							addFinishedQueue(chunkXZ);
						}
						// Else, mesh is nullptr
					}
					// Else, chunk is nullptr
				}
				// Else, has no chunk
			}
			else if (flag == 2)
			{

				std::cout << "(" << chunkXZ.x << ", " << chunkXZ.y << "): Load" << std::endl;

				// There must be a chunk. Chunk loader creates empty chunk.
				bool hasChunk = map->hasChunkAtXZ(chunkXZ.x, chunkXZ.y);
				if (hasChunk)
				{
					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
					if (chunk)
					{
						if (!chunk->isGenerated())
						{
							//std::cout << "Thraed #" << std::this_thread::get_id() << " generating chunk" << std::endl;
							chunk->generate();
						}

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

void Voxel::ChunkWorkManager::createThread(ChunkMap* map, ChunkMeshGenerator* chunkMeshGenerator)
{
	if (running)
	{
		meshBuilderThread = std::thread(&ChunkWorkManager::processChunk, this, map, chunkMeshGenerator);
	}
}

void Voxel::ChunkWorkManager::notify()
{
	cv.notify_one();
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