#include "ChunkWorkManager.h"
#include <iostream>
#include <World.h>
#include <Region.h>
#include <Terrain.h>
#include <ChunkMap.h>
#include <Chunk.h>
#include <ChunkUtil.h>
#include <ChunkSection.h>
#include <ChunkMeshGenerator.h>
#include <ChunkMesh.h>
#include <Utility.h>
#include <unordered_set>
#include <Color.h>
#include <HeightMap.h>
#include <TreeBuilder.h>

using namespace Voxel;

ChunkWorkManager::ChunkWorkManager()
{
	running.store(false);
	firstInitDone.store(false);
	workState.store(WORK_STATE::IDLE);
}

void Voxel::ChunkWorkManager::addPreGenerateWork(const glm::ivec2 & coordinate, const bool highPriority)
{	
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	if (highPriority)
	{
		preGenerateQueue.push_front(coordinate);
	}
	else
	{
		preGenerateQueue.push_back(coordinate);
	}

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addPreGenerateWorks(const std::vector<glm::ivec2>& coordinates, const bool highPriority)
{	
	//auto start = Utility::Time::now();
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	if (highPriority)
	{
		for (auto xz : coordinates)
		{
			preGenerateQueue.push_front(xz);
		}
	}
	else
	{
		for (auto xz : coordinates)
		{
			preGenerateQueue.push_back(xz);
		}
	}

	//auto end = Utility::Time::now();
	//std::cout << "addLoad() took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addSmoothWork(const glm::ivec2 & coordinate, const bool highPriority)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	if (highPriority)
	{
		smoothQueue.push_front(coordinate);
	}
	else
	{
		smoothQueue.push_back(coordinate);
	}

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addStructureWork(const glm::ivec2 & coordinate, const bool highPriority)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	if (highPriority)
	{
		addStructureQueue.push_front(coordinate);
	}
	else
	{
		addStructureQueue.push_back(coordinate);
	}

	cv.notify_one();
}

void Voxel::ChunkWorkManager::addGenerateWork(const glm::ivec2 & coordinate, const bool highPriority)
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

void Voxel::ChunkWorkManager::addGenerateWorks(const std::vector<glm::ivec2>& coordinates, const bool highPriority)
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

void Voxel::ChunkWorkManager::sortBuildMeshQueue(const glm::ivec2& currentChunkXZ)
{
	glm::vec2 p = glm::vec2(currentChunkXZ);

	std::vector<glm::vec2> loadQueueFloat;

	for (auto xz : buildMeshQueue)
	{
		loadQueueFloat.push_back(glm::vec2(xz));
	}

	std::sort(loadQueueFloat.begin(), loadQueueFloat.end(), [p](const glm::vec2& lhs, const glm::vec2& rhs) { return glm::distance(p, lhs) < glm::distance(p, rhs); });

	buildMeshQueue.clear();

	for (auto xz : loadQueueFloat)
	{
		buildMeshQueue.push_back(glm::ivec2(xz));
	}
}

void Voxel::ChunkWorkManager::sortBuildMeshQueue(const glm::ivec2 & currentChunkXZ, const std::vector<glm::ivec2>& visibleChunks)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	glm::vec2 p = glm::vec2(currentChunkXZ);

	std::vector<glm::vec2> loadQueueFloat;

	for (auto xz : buildMeshQueue)
	{
		loadQueueFloat.push_back(glm::vec2(xz));
	}
	
	std::sort(loadQueueFloat.begin(), loadQueueFloat.end(), [p, visibleChunks](const glm::vec2& lhs, const glm::vec2& rhs)
	{
		bool lhsVisible = false;
		glm::ivec2 iLhs = glm::ivec2(lhs);

		for (auto& e : visibleChunks)
		{
			if (e == iLhs)
			{
				lhsVisible = true;
				break;
			}
		}

		bool rhsVisible = false;
		glm::ivec2 iRhs = glm::ivec2(rhs);

		for (auto& e : visibleChunks)
		{
			if (e == iRhs)
			{
				rhsVisible = true;
				break;
			}
		}

		if (lhsVisible && !rhsVisible)
		{
			return true;
		}
		else if (!lhsVisible && rhsVisible)
		{
			return false;
		}
		else if (lhsVisible && rhsVisible)
		{
			return glm::distance(p, lhs) < glm::distance(p, rhs);
		}
		else
		{
			return glm::distance(p, lhs) < glm::distance(p, rhs);
		}
	});

	buildMeshQueue.clear();

	for (auto xz : loadQueueFloat)
	{
		buildMeshQueue.push_back(glm::ivec2(xz));
	}
}

void Voxel::ChunkWorkManager::sortBuildMeshQueue(const glm::ivec2 & currentChunkXZ, const std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs>& visibleChunks)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	glm::vec2 p = glm::vec2(currentChunkXZ);

	std::vector<glm::vec2> loadQueueFloat;

	for (auto xz : buildMeshQueue)
	{
		loadQueueFloat.push_back(glm::vec2(xz));
	}

	std::sort(loadQueueFloat.begin(), loadQueueFloat.end(), [p, visibleChunks](const glm::vec2& lhs, const glm::vec2& rhs)
	{
		bool lhsVisible = visibleChunks.find(glm::ivec2(lhs)) != visibleChunks.end();
		bool rhsVisible = visibleChunks.find(glm::ivec2(rhs)) != visibleChunks.end();

		if (lhsVisible && !rhsVisible)
		{
			return true;
		}
		else if (!lhsVisible && rhsVisible)
		{
			return false;
		}
		else if (lhsVisible && rhsVisible)
		{
			return glm::distance(p, lhs) < glm::distance(p, rhs);
		}
		else
		{
			return glm::distance(p, lhs) < glm::distance(p, rhs);
		}
	});

	buildMeshQueue.clear();

	for (auto xz : loadQueueFloat)
	{
		buildMeshQueue.push_back(glm::ivec2(xz));
	}
}

void Voxel::ChunkWorkManager::addFinishedQueue(const glm::ivec2 & coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(finishedQueueMutex);
	//std::cout << "Finished unloading (" << coordinate.x << ", " << coordinate.y << ")\n";
	unloadFinishedQueue.push_back(coordinate);
}

bool Voxel::ChunkWorkManager::getUnloadFinishedQueueFront(glm::ivec2& coordinate)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(finishedQueueMutex);

	if (!unloadFinishedQueue.empty())
	{
		coordinate = unloadFinishedQueue.front();
		//std::cout << "Main thread has (" << coordinate.x << ", " << coordinate.y << ") to unload\n";
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

std::string Voxel::ChunkWorkManager::getDebugOutput()
{
	std::string log = "";

	{
		// Scope lock
		std::unique_lock<std::mutex> lock(queueMutex);

		log += "P: " + std::to_string(preGenerateQueue.size()) + " / ";
		log += "S: " + std::to_string(smoothQueue.size()) + " / ";
		log += "G: " + std::to_string(generateQueue.size()) + " / ";
		log += "A: " + std::to_string(addStructureQueue.size()) + " / ";
		log += "B: " + std::to_string(buildMeshQueue.size()) + " / ";
	}

	{
		// Scope lock
		std::unique_lock<std::mutex> lock(finishedQueueMutex);

		log += "F: " + std::to_string(unloadFinishedQueue.size());
	}

	return log;
}


void Voxel::ChunkWorkManager::work(ChunkMap* map, ChunkMeshGenerator* meshGenerator, World* world)
{
	// loop while it's running
	//std::cout << "Thraed #" << std::this_thread::get_id() << " started to build mesh \n";
	while (running)
	{
		{
			// Scope lock
			std::unique_lock<std::mutex> fLock(finishedQueueMutex);
			// Threads need to wait until ChunkMap releases the chunk, which modifies std::unordered_map.
			// It's not safe to get chunk pointer from map while mainthread modifies the map.
			while (!unloadFinishedQueue.empty() && workState.load() != WORK_STATE::CLEARING)
			{
				cv.wait(fLock);
			}

			//std::cout << "No need to wait!\n";
		}

		if (workState.load() == WORK_STATE::CLEARING)
		{
			// clear all work

			// Scope lock
			std::unique_lock<std::mutex> lock(queueMutex);

			std::cout << "Clearing all the work\n";

			/*
			for (auto& chunkXZ : preGenerateQueue)
			{
				auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
				if (chunk)
				{
					auto mesh = chunk->getMesh();
					if (mesh)
					{
						// Scope lock
						std::unique_lock<std::mutex> fLock(finishedQueueMutex);

						unloadFinishedQueue.push_back(chunkXZ);
					}
				}
			}

			for (auto& chunkXZ : smoothQueue)
			{
				auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
				if (chunk)
				{
					auto mesh = chunk->getMesh();
					if (mesh)
					{
						// Scope lock
						std::unique_lock<std::mutex> fLock(finishedQueueMutex);

						unloadFinishedQueue.push_back(chunkXZ);
					}
				}
			}

			for (auto& chunkXZ : generateQueue)
			{
				auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
				if (chunk)
				{
					auto mesh = chunk->getMesh();
					if (mesh)
					{
						// Scope lock
						std::unique_lock<std::mutex> fLock(finishedQueueMutex);

						unloadFinishedQueue.push_back(chunkXZ);
					}
				}
			}

			for (auto& chunkXZ : addStructureQueue)
			{
				auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
				if (chunk)
				{
					auto mesh = chunk->getMesh();
					if (mesh)
					{
						// Scope lock
						std::unique_lock<std::mutex> fLock(finishedQueueMutex);

						unloadFinishedQueue.push_back(chunkXZ);
					}
				}
			}

			for (auto& chunkXZ : buildMeshQueue)
			{
				auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
				if (chunk)
				{
					auto mesh = chunk->getMesh();
					if (mesh)
					{
						// Scope lock
						std::unique_lock<std::mutex> fLock(finishedQueueMutex);

						unloadFinishedQueue.push_back(chunkXZ);
					}
				}
			}
			*/

			// empty all the queue. 
			preGenerateQueue.clear();
			smoothQueue.clear();
			generateQueue.clear();
			addStructureQueue.clear();
			buildMeshQueue.clear();

			workState.store(WORK_STATE::WAITING_MAIN_THREAD);
		}
		else if (workState.load() == WORK_STATE::RUNNING)
		{
			glm::ivec2 chunkXZ;
			WorkType workType = WorkType::IDLE;

			{
				// Scope lock
				std::unique_lock<std::mutex> lock(queueMutex);

				// wait if both queue are empty. Must be running.
				while (running && preGenerateQueue.empty() && addStructureQueue.empty() && smoothQueue.empty() && generateQueue.empty() && buildMeshQueue.empty() && workState.load() == WORK_STATE::RUNNING)
				{
					cv.wait(lock);
				}

				//std::cout << "There is job to do!\n";

				if (running == false)
				{
					// quit
					break;
				}

				// In order of preGen > smooth > gen > build mesh
				if (!preGenerateQueue.empty())
				{
					chunkXZ = preGenerateQueue.front();
					preGenerateQueue.pop_front();
					workType = WorkType::PRE_GENERATE;
				}
				else if (!smoothQueue.empty())
				{
					chunkXZ = smoothQueue.front();
					smoothQueue.pop_front();
					workType = WorkType::SMOOTH;
				}
				// If there is nothing to pregenerate, generate chunk. (generates blocks)
				else if (!generateQueue.empty())
				{
					chunkXZ = generateQueue.front();
					generateQueue.pop_front();
					workType = WorkType::GENERATE;
				}
				else if (!addStructureQueue.empty())
				{
					chunkXZ = addStructureQueue.front();
					addStructureQueue.pop_front();
					workType = WorkType::ADD_STRUCTURE;

					if (addStructureQueue.empty())
					{
						sortBuildMeshQueue(map->getCurrentChunkXZ());
						if (firstInitDone.load() == false)
						{
							firstInitDone.store(true);
						}
					}
				}
				// If there is nothing to generate, start to build mesh
				else if (!buildMeshQueue.empty())
				{
					chunkXZ = buildMeshQueue.front();
					buildMeshQueue.pop_front();
					workType = WorkType::BUILD_MESH;
				}
				//Else, flag is 0. 
				else
				{
					continue;
				}
			}


			if (map && meshGenerator)
			{
				//std::cout << "(" << chunkXZ.x << ", " << chunkXZ.y << ")\n";

				if (workType == WorkType::PRE_GENERATE)
				{
					//auto start = Utility::Time::now();
					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
					if (chunk)
					{
						//std::cout << "PreGen\n";
						// Get all block world position in XZ axises and find which region the are at
						std::vector<unsigned int> regionMap(Constant::CHUNK_SECTION_WIDTH * Constant::CHUNK_SECTION_LENGTH, -1);

						const float step = 1.0f;	// Block size

						float x = (chunkXZ.x * Constant::CHUNK_BORDER_SIZE);
						float z = (chunkXZ.y * Constant::CHUNK_BORDER_SIZE);

						std::unordered_set<unsigned int> regionIDSet;
						//std::unordered_set<Voxel::TerrainType> terrainTypeSet;

						// iterate all blocks in x and z axis
						for (int i = 0; i < Constant::CHUNK_SECTION_WIDTH; i++)
						{
							for (int j = 0; j < Constant::CHUNK_SECTION_LENGTH; j++)
							{
								glm::vec2 blockXZPos = glm::vec2(x + 0.5f, z + 0.5f);

								// First, check if block is in region that we found
								//unsigned int regionID = world->findClosestRegionToPoint(blockXZPos);
								bool inBoundary = world->isPointInBoundary(blockXZPos);
								unsigned int regionID = -1;

								if (inBoundary)
								{
									//regionID = world->findRegionHasPoint(blockXZPos);
									// find closest region
									unsigned int closestRegionID = world->findClosestRegionToPoint(blockXZPos);
									// get the region
									auto region = world->getRegion(closestRegionID);

									// Check if closest region has block 
									if (region->isPointIsInRegion(blockXZPos))
									{
										// found
										regionID = closestRegionID;
									}
									else
									{
										// Nope, check if neighbor regions has block
										if (region->isPointIsInRegionNeighbor(blockXZPos, regionID))
										{
											// found
										}
										else
										{
											// hmm
											assert(false);
										}
									}
								}

								// convert to index
								auto index = static_cast<int>(i + (Constant::CHUNK_SECTION_WIDTH * j));

								// This method can't be fail. It will must find closest region unless block is out of boundary
								regionMap.at(index) = regionID;

								// step z.
								z += step;

								// Save region id in set. 
								auto find_it = regionIDSet.find(regionID);
								if (find_it == regionIDSet.end())
								{
									// New region ID
									if (inBoundary)
									{
										if (regionID == -1)
										{
											// Failed to find region
											assert(false);
										}
										else
										{
											// block is not out of boundary. 
											// Get terarin type of region
											auto terrainType = world->getRegion(regionID)->getTerrainType();
											// Save it
											map->setRegionTerrainType(regionID, terrainType);
											//terrainTypeSet.emplace(terrainType.getType());
										}
									}
									else
									{
										// block is out of boundary
										map->setRegionTerrainType(-1, Terrain());
									}

									// Save region ID
									regionIDSet.emplace(regionID);
								}
							}

							// step x and reset z
							x += step;
							z = (chunkXZ.y * Constant::CHUNK_BORDER_SIZE);
						}
						
						// Generate height map.
						HeightMap::generateHeightMapForChunk(chunk->getPosition(), chunk->heightMap, regionMap, map->getRegionTerrainsMap());

						std::vector<std::vector<int>> plainHeightMap;
						HeightMap::generatePlainHeightMapForChunk(chunk->getPosition(), plainHeightMap);

						chunk->mergeHeightMap(plainHeightMap);

						chunk->smoothed = false;

						if (regionIDSet.size() == 1)
						{
							// There is only 1 region in this chunk.
							chunk->setRegionMap(regionMap.front());
						}
						else
						{
							// Multiple region exists in this chunk. save region map to chunk
							chunk->setRegionMap(regionMap);
						}

						addSmoothWork(chunkXZ);
					}
					//auto end = Utility::Time::now();
					//std::cout << "pg t: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
				}
				else if (workType == WorkType::SMOOTH)
				{
					// There must be a chunk. Chunk loader creates empty chunk.
					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
					if (chunk)
					{
						//std::cout << "Thraed #" << std::this_thread::get_id() << " generating chunk\n";
						//auto s = Utility::Time::now();

						if (chunk->hasMultipleRegion())
						{
							//std::cout << "Smooth\n";
							if (chunk->isActive())
							{
								std::vector<std::vector<std::shared_ptr<Chunk>>> nearByChunks = map->getNearByChunks(chunkXZ);

								const int q11 = nearByChunks.at(2).at(2)->getQ22();
								const int q12 = nearByChunks.at(0).at(2)->getQ21();
								const int q21 = nearByChunks.at(2).at(0)->getQ12();
								const int q22 = nearByChunks.at(0).at(0)->getQ11();

								chunk->smoothed = true;
								HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 0, 0, Constant::CHUNK_SECTION_WIDTH, Constant::CHUNK_SECTION_LENGTH);

								/*
								// Need correct q11, q12, q21, q22
								HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 0, 0, 8, 8);
								HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 8, 0, 16, 8);
								HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 0, 8, 8, 16);
								HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 8, 8, 16, 16);
								*/
							}
						}

						if (!map->isChunkOnEdge(chunkXZ))
						{
							// Only generate chunk that is not on edge
							addGenerateWork(chunkXZ);
						}
					}
				}
				else if (workType == WorkType::GENERATE)
				{
					// There must be a chunk. Chunk loader creates empty chunk.
					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
					if (chunk)
					{
						if (!chunk->isGenerated() && chunk->isActive())
						{
							//std::cout << "Gen\n";

							//std::cout << "Thraed #" << std::this_thread::get_id() << " generating chunk\n";
							//auto s = Utility::Time::now();

							// Check if chunk is smoothed
							if (!chunk->smoothed)
							{
								// Chunk is not smoothed. Check nearby chunk and see if chunk needs to be smoothed
								std::vector<std::vector<std::shared_ptr<Chunk>>> nearByChunks = map->getNearByChunks(chunkXZ);

								int needSmooth = 0;

								const int cq11 = chunk->getQ11();
								const int cq12 = chunk->getQ12();
								const int cq21 = chunk->getQ21();
								const int cq22 = chunk->getQ22();

								const int diff = 3;

								auto& EChunk = nearByChunks.at(1).at(0);
								if (EChunk)
								{
									if (EChunk->isSmoothed())
									{
										const int ecQ12 = EChunk->getQ12();
										const int ecQ11 = EChunk->getQ11();

										const int diffQ12 = glm::abs(ecQ12 - cq22);
										const int diffQ11 = glm::abs(ecQ11 - cq21);

										if (diffQ12 > diff || diffQ11 > diff)
										{
											needSmooth++;
										}
									}
								}

								auto& WChunk = nearByChunks.at(1).at(2);
								if (WChunk)
								{
									if (WChunk->isSmoothed())
									{
										const int wcQ22 = WChunk->getQ22();
										const int wcQ21 = WChunk->getQ21();

										const int diffQ22 = glm::abs(wcQ22 - cq12);
										const int diffQ21 = glm::abs(wcQ21 - cq11);

										if (diffQ22 > diff || diffQ21 > diff)
										{
											needSmooth++;
										}
									}
								}

								auto& SChunk = nearByChunks.at(0).at(1);
								if (SChunk)
								{
									if (SChunk->isSmoothed())
									{
										const int scQ11 = SChunk->getQ11();
										const int scQ21 = SChunk->getQ21();

										const int diffQ11 = glm::abs(scQ11 - cq12);
										const int diffQ21 = glm::abs(scQ21 - cq22);

										if (diffQ11 > diff || diffQ21 > diff)
										{
											needSmooth++;
										}
									}
								}

								auto& NChunk = nearByChunks.at(2).at(1);
								if (NChunk)
								{
									if (NChunk->isSmoothed())
									{
										const int ncQ22 = NChunk->getQ22();
										const int ncQ12 = NChunk->getQ12();

										const int diffQ22 = glm::abs(ncQ22 - cq21);
										const int diffQ12 = glm::abs(ncQ12 - cq11);
										if (diffQ22 > diff || diffQ12 > diff)
										{
											needSmooth++;
										}
									}
								}

								if (needSmooth >= 1)
								{
									//std::cout << "Gen smooth\n";
									const int q11 = nearByChunks.at(2).at(2)->getQ22();
									const int q12 = nearByChunks.at(0).at(2)->getQ21();
									const int q21 = nearByChunks.at(2).at(0)->getQ12();
									const int q22 = nearByChunks.at(0).at(0)->getQ11();

									HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 0, 0, Constant::CHUNK_SECTION_WIDTH, Constant::CHUNK_SECTION_LENGTH);
								}
							}

							chunk->preGenerateChunkSections(2, chunk->findMaxY() / Constant::CHUNK_SECTION_HEIGHT);

							// All chunks starts from chunk section 3 because sea level starts at 33.
							chunk->generate();

							int rand = Utility::Random::randomInt100();

							int treeChance = 70;
							if (rand < treeChance)
							{
								addStructureWork(chunkXZ);
							}
							else
							{
								addBuildMeshWork(chunkXZ);
							}

							//auto e = Utility::Time::now();
							//std::cout << "Chunk generation took: " << Utility::Time::toMilliSecondString(s, e) << std::endl;
						}
					}
					// Else, chunk is nullptr
				}
				else if (workType == WorkType::ADD_STRUCTURE)
				{
					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
					if (chunk && chunk->isActive())
					{
						// First, add small structures like grass, flower, etc
						// Then, add mid sized structures like stone, 
						// Then, check if chunk has structure. If so, add structure
						// Then, add trees. Point is, tree is large and can have up to 1 tree per chunk. We don't want to add tree where large structure exists.

						// test tree
						glm::ivec2 treeLocalPos = HeightMap::getTreePosition(chunk->getPosition());
						//glm::ivec2 treePos = glm::ivec2(Utility::Random::randomInt(3, 13), Utility::Random::randomInt(3, 13));

						// Don't spawn tree at the edge of chunk. 
						//treePos = glm::clamp(treePos, 5, 11);

						int treeY = chunk->heightMap.at(treeLocalPos.x).at(treeLocalPos.y) + 1;

						TreeBuilder::TrunkHeight trunkHeight;

						int hRand = Utility::Random::randomInt100();
						if (hRand > 65)
						{
							trunkHeight = TreeBuilder::TrunkHeight::SMALL;
						}
						else if (hRand <= 65 && hRand > 33)
						{
							trunkHeight = TreeBuilder::TrunkHeight::MEDIUM;
						}
						else if (hRand <= 33)
						{
							trunkHeight = TreeBuilder::TrunkHeight::LARGE;
						}

						// Get tree width. 
						TreeBuilder::TrunkWidth trunkWidth;

						int wRand = Utility::Random::randomInt100();
						if (wRand > 65)
						{
							trunkWidth = TreeBuilder::TrunkWidth::SMALL;
						}
						else if (wRand <= 65 && wRand > 33)
						{
							trunkWidth = TreeBuilder::TrunkWidth::MEDIUM;
						}
						else if (wRand <= 33)
						{
							trunkWidth = TreeBuilder::TrunkWidth::LARGE;
						}

						//auto treeStart = Utility::Time::now();
						//TreeBuilder::createTree(TreeBuilder::TreeType::OAK, trunkHeight, trunkWidth, map, chunkXZ, treeLocalPos, treeY);
						//auto treeEnd = Utility::Time::now();

						//std::cout << "tree t: " << Utility::Time::toMicroSecondString(treeStart, treeEnd) << std::endl;

						addBuildMeshWork(chunkXZ);
					}
				}
				else if (workType == WorkType::BUILD_MESH)
				{
					//std::cout << "BuildMesh";
					//auto s = Utility::Time::now();

					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
					if (chunk)
					{
						if (chunk->isGenerated())
						{
							auto mesh = chunk->getMesh();
							if (mesh)
							{
								// There can be two cases. 
								// 1. Chunk is newly generated and need mesh.
								// 2. Chunk already has mesh but need to refresh
								//auto s = Utility::Time::now();
								meshGenerator->generateSingleChunkMesh(chunk.get(), map);
								//std::cout << "Done\n";
								//auto e = Utility::Time::now();
								//std::cout << "m t: " << Utility::Time::toMilliSecondString(s, e) << std::endl;
							}
							// Else, mesh is nullptr
						}
						// Else, chunk is not generated
					}
					// Else, chunk is nullptr

					//auto e = Utility::Time::now();
					//std::cout << "Chunk mesh build took: " << Utility::Time::toMilliSecondString(s, e) << std::endl;
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

	std::cout << "[ChunkWorkManager] Spawning " << threadCount << " thread(s)\n";
	
	// for now, just use 1 thread. Using more than 1 thread doesn't really improves the loading performance
	// (I guess because if mutex lock)
	if (running)
	{
		for (int i = 0; i < threadCount; i++)
		{
			workerThreads.push_back(std::thread(&ChunkWorkManager::work, this, map, meshGenerator, world));
		}
	}

	workState.store(WORK_STATE::RUNNING);
}

bool Voxel::ChunkWorkManager::isFirstInitDone()
{
	return firstInitDone.load();
}

void Voxel::ChunkWorkManager::clear()
{
	workState.store(WORK_STATE::CLEARING);
}

bool Voxel::ChunkWorkManager::isClearing()
{
	return workState.load() == WORK_STATE::CLEARING;
}

bool Voxel::ChunkWorkManager::isWaitingMainThread()
{
	return workState.load() == WORK_STATE::WAITING_MAIN_THREAD;
}

void Voxel::ChunkWorkManager::resumeWork()
{
	workState.store(WORK_STATE::RUNNING);
}

bool Voxel::ChunkWorkManager::isGeneratingChunks()
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	return (preGenerateQueue.empty() == false) || (smoothQueue.empty() == false) || (generateQueue.empty() == false) || (addStructureQueue.empty() == false);
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
		//std::cout << "Waiting to thread join...\n";
		// Scope lock
		std::unique_lock<std::mutex> lock(queueMutex);
		preGenerateQueue.clear();
		generateQueue.clear();
		smoothQueue.clear();
		buildMeshQueue.clear();
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