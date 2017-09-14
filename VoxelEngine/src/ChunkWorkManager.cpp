#include "ChunkWorkManager.h"
#include <iostream>
#include <World.h>
#include <ChunkMap.h>
#include <Chunk.h>
#include <ChunkUtil.h>
#include <ChunkSection.h>
#include <ChunkMeshGenerator.h>
#include <ChunkMesh.h>
#include <Utility.h>
#include <unordered_set>
#include <Color.h>

using namespace Voxel;

ChunkWorkManager::ChunkWorkManager()
{
	running.store(false);
}

void Voxel::ChunkWorkManager::addLoad(const glm::ivec2 & coordinate, const bool highPriority)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	if (highPriority)
	{
		generateQueue.push_front(coordinate);
	}
	else
	{
		generateQueue.push_back(coordinate);
	}

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addLoads(const std::vector<glm::ivec2>& coordinates, const bool highPriority)
{
	//auto start = Utility::Time::now();
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	if (highPriority)
	{
		for (auto xz : coordinates)
		{
			generateQueue.push_front(xz);
		}
	}
	else
	{
		for (auto xz : coordinates)
		{
			generateQueue.push_back(xz);
		}
	}

	//auto end = Utility::Time::now();
	//std::cout << "addLoad() took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addBuildMeshWork(const glm::ivec2 & coordinate, const bool highPriority)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	if (highPriority)
	{
		buildMeshQueue.push_front(coordinate);
	}
	else
	{
		buildMeshQueue.push_back(coordinate);
	}

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addBuildMeshWorks(const std::vector<glm::ivec2>& coordinates, const bool highPriority)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	if (highPriority)
	{
		for (auto xz : coordinates)
		{
			buildMeshQueue.push_front(xz);
		}
	}
	else
	{
		for (auto xz : coordinates)
		{
			buildMeshQueue.push_back(xz);
		}
	}

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addUnload(const glm::ivec2 & coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	unloadQueue.push_back(coordinate);
	generateQueue.remove(coordinate);

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addUnloads(const std::vector<glm::ivec2>& coordinates)
{
	//auto start = Utility::Time::now();
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs> lut(coordinates.begin(), coordinates.end());

	for (auto xz : coordinates)
	{
		unloadQueue.push_back(xz);
	}

	auto it = generateQueue.begin();
	for (; it != generateQueue.end();)
	{
		auto find_it = lut.find(*it);
		if (find_it != lut.end())
		{
			it = generateQueue.erase(it);
		}
		else
		{
			it++;
		}
	}

	//auto end = Utility::Time::now();
	//std::cout << "addUnload() took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;

	//cv.notify_one();
}

void Voxel::ChunkWorkManager::sortLoadQueue(const glm::vec3 & playerPosition)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	// Shift by 1 if player is in negative position in XZ axis.
	if (playerPosition.x < 0) chunkX -= 1;
	if (playerPosition.z < 0) chunkZ -= 1;

	glm::vec2 p = glm::vec2(chunkX, chunkZ);

	std::vector<glm::vec2> loadQueueFloat;

	for (auto xz : generateQueue)
	{
		loadQueueFloat.push_back(glm::vec2(xz));
	}

	std::sort(loadQueueFloat.begin(), loadQueueFloat.end(), [p](const glm::vec2& lhs, const glm::vec2& rhs) { return glm::distance(p, lhs) < glm::distance(p, rhs); });

	generateQueue.clear();

	for (auto xz : loadQueueFloat)
	{
		generateQueue.push_back(glm::ivec2(xz));
	}
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
		//std::cout << "Main thread has (" << coordinate.x << ", " << coordinate.y << ") to unload" << std::endl;
		return true;
	}

	return false;
}

void Voxel::ChunkWorkManager::popFinishedAndNotify()
{
	// Scope lock
	std::unique_lock<std::mutex> lock(finishedQueueMutex);

	if (!unloadFinishedQueue.empty())
	{
		unloadFinishedQueue.pop_front();
	}

	if (unloadFinishedQueue.empty())
	{
		cv.notify_one();
	}
}

void Voxel::ChunkWorkManager::work(ChunkMap* map, ChunkMeshGenerator* meshGenerator, World* world)
{
	// loop while it's running
	//std::cout << "Thraed #" << std::this_thread::get_id() << " started to build mesh " << std::endl;
	while (running)
	{
		{
			// Scope lock
			std::unique_lock<std::mutex> fLock(finishedQueueMutex);
			// Threads need to wait until ChunkMap releases the chunk, which modifies std::unordered_map.
			// It's not safe to get chunk pointer from map while mainthread modifies the map.
			while (!unloadFinishedQueue.empty())
			{
				cv.wait(fLock);
			}

			//std::cout << "No need to wait!" << std::endl;
		}

		glm::ivec2 chunkXZ;
		int flag = IDLE_WORK;

		{
			// Scope lock
			std::unique_lock<std::mutex> lock(queueMutex);

			// wait if both queue are empty. Must be running.
			while (running && generateQueue.empty() && unloadQueue.empty() && buildMeshQueue.empty())
			{
				cv.wait(lock);
			}

			//std::cout << "There is job to do!" << std::endl;

			if (running == false)
			{
				// quit
				break;
			}

			// Unloading goes first
			if (!unloadQueue.empty())
			{
				chunkXZ = unloadQueue.front();
				unloadQueue.pop_front();
				flag = UNLOAD_WORK;
			}
			// If there is nothing to unload, start to load
			else if (!generateQueue.empty())
			{
				chunkXZ = generateQueue.front();
				generateQueue.pop_front();
				flag = GENERATE_WORK;
			}
			// If there is nothing to generate, start to build mesh
			else if (!buildMeshQueue.empty())
			{
				chunkXZ = buildMeshQueue.front();
				buildMeshQueue.pop_front();
				flag = BUILD_MESH_WORK;
			}
			//Else, flag is 0. 
			else
			{
				continue;
			}
		}


		if (map && meshGenerator)
		{
			if (flag == UNLOAD_WORK)
			{
				//std::cout << "(" << chunkXZ.x << ", " << chunkXZ.y << "): Unload" << std::endl;

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
			else if (flag == GENERATE_WORK)
			{
				//std::cout << "(" << chunkXZ.x << ", " << chunkXZ.y << "): Load" << std::endl;

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
							auto s = Utility::Time::now();

							// Get chunk's AABB
							AABB boundingBox = chunk->getBoundingBox();

							bool needMesh = false;

							// hmm?
							//boundingBox.size -= 1.0f;

							// Get all regions that chunk is in.
							std::vector<unsigned int> regionIDs;
							world->findAllRegionsWithAABB(boundingBox, regionIDs);

							if (!regionIDs.empty())
							{
								auto size = regionIDs.size();
								if (size == 1)
								{
									// completely in region. 
									chunk->setAllBlockRegion(regionIDs.front());
									needMesh = true;
								}
								else
								{
									// In multiple regions
									// Get all block world position in XZ axises and find which region the are at
									std::vector<unsigned int> blockRegions(Constant::CHUNK_SECTION_WIDTH * Constant::CHUNK_SECTION_LENGTH, -1);

									const float step = 1.0f;

									float x = (chunkXZ.x * Constant::CHUNK_BORDER_SIZE);
									float z = (chunkXZ.y * Constant::CHUNK_BORDER_SIZE);

									for (int i = 0; i < Constant::CHUNK_SECTION_WIDTH; i++)
									{
										for (int j = 0; j < Constant::CHUNK_SECTION_LENGTH; j++)
										{
											glm::vec2 blockXZPos = glm::vec2(x + 0.5f, z + 0.5f);

											auto index = static_cast<int>(i + (Constant::CHUNK_SECTION_WIDTH * j));

											unsigned int regionID = -1;
											world->findFirstRegionHasPoint(blockXZPos, regionID);

											if (regionID == -1)
											{
												std::cout << "RegionID is -1. Chunk (" << chunkXZ.x << ", " << chunkXZ.y << "), block (" << blockXZPos.x << ", " << blockXZPos.y << ")" << std::endl;
												// AFAIK, this happens when point is on the edge of polygon.
												// In this case, we can do either
												// 1) iterate all region and find closest region
												// 2) use chunk's center position to find region
												// For now, I'm going to use first method
												regionID = world->findClosestRegionToPoint(blockXZPos);
												std::cout << "Resolved to closest region: " << regionID << std::endl;
											}

											blockRegions.at(index) = regionID;
											/*
											for (auto id : regionIDs)
											{
												if (world->isPointInRegion(id, blockXZPos))
												{
													blockRegions.at(index) = id;
													break;
												}
												else
												{
													// Check neighbors too
													unsigned int neighborID = -1;
													if (world->isPointInRegionNeighbor(id, blockXZPos, neighborID))
													{
														blockRegions.at(index) = neighborID;
														break;
													}
												}
											}
											*/

											z += step;
										}

										x += step;
										z = (chunkXZ.y * Constant::CHUNK_BORDER_SIZE);
									}

									chunk->setBlockRegion(blockRegions);
									needMesh = true;
								}

								chunk->generate();
							}




							// Else, it's empty. do nothing

							/*
							// Check which region is this chunk at
							unsigned int regionID = -1;
							bool success = world->findRegionWithAABB(boundingBox, regionID);

							if (success)
							{
								int difficulty = world->getRegionDifficulty(regionID);
								chunk->generateWithColor(Color::getDifficultyColor(difficulty));
								needMesh = true;
							}
							else
							{
								chunk->generateWithColor(glm::uvec3(255));
								needMesh = true;
							}
							*/

							if (needMesh)
							{
								addBuildMeshWork(chunkXZ);
							}

							auto e = Utility::Time::now();
							std::cout << "Chunk generation took: " << Utility::Time::toMilliSecondString(s, e) << std::endl;
						}
					}
					// Else, chunk is nullptr
				}
				// Else, has no chunk
			}
			else if (flag == BUILD_MESH_WORK)
			{
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
							// There can be two cases. 
							// 1. Chunk is newly generated and need mesh.
							// 2. Chunk already has mesh but need to refresh
							//auto s = Utility::Time::now();
							meshGenerator->generateSingleChunkMesh(chunk.get(), map);
							//auto e = Utility::Time::now();
							//std::cout << "m t: " << Utility::Time::toMilliSecondString(s, e) << std::endl;
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

void Voxel::ChunkWorkManager::createThreads(ChunkMap* map, ChunkMeshGenerator* meshGenerator, World* world, const int coreCount)
{
	// Get number of thread to spawn
	// 1 for main thread
	int threadCount = 0;
	if (coreCount == 1 || coreCount == 2)
	{
		// For single and dual, spawn 1 thread
		threadCount = 1;
	}
	else if(threadCount == 4)
	{
		// For quad core, spawn 2
		threadCount = 2;
	}
	else
	{
		// For more than quad core, spawn 3
		threadCount = 3;
	}

	// Debug. For now, just use 1 thread
	threadCount = 1;

	std::cout << "[ChunkWorkManager] Spawning " << threadCount << " thread(s)" << std::endl;
	
	// for now, just use 1 thread. Using more than 1 thread doesn't really improves the loading performance
	// (I guess because if mutex lock)
	if (running)
	{
		for (int i = 0; i < threadCount; i++)
		{
			workerThreads.push_back(std::thread(&ChunkWorkManager::work, this, map, meshGenerator, world));
		}
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
		generateQueue.clear();
		unloadQueue.clear();
	}

	cv.notify_one();

	{
		std::unique_lock<std::mutex> lock(finishedQueueMutex);
		unloadFinishedQueue.clear();
	}

	cv.notify_one();

	for (auto& thread : workerThreads)
	{
		if (thread.joinable())
		{
			std::cout << "joining thread #" << thread.get_id() << std::endl;
			thread.join();
		}
	}
}