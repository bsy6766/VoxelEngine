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
#include <Random.h>
#include <unordered_set>
#include <Color.h>
#include <HeightMap.h>
#include <TreeBuilder.h>

using namespace Voxel;

bool Voxel::ChunkWorkManager::isAllWorkQueueEmpty()
{
	return preGenerateQueue.empty() && addStructureQueue.empty() && smoothQueue.empty() && generateQueue.empty() && buildMeshQueue.empty() && refreshMeshQueue.empty();
}

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

void Voxel::ChunkWorkManager::addRefreshWork(const glm::ivec2 & coordinate, const bool highPriority)
{
	// Scope lock
	std::unique_lock<std::mutex> lock(queueMutex);

	if (highPriority)
	{
		refreshMeshQueue.push_front(coordinate);
	}
	else
	{
		refreshMeshQueue.push_back(coordinate);
	}

	cv.notify_one();
}

void Voxel::ChunkWorkManager::sortBuildMeshQueue(const glm::ivec2& currentChunkXZ)
{
	glm::vec2 p = glm::vec2(currentChunkXZ);

	//std::cout << "Sorting build mesh work with (" << currentChunkXZ.x << ", " << currentChunkXZ.y << ")" << std::endl;

	std::vector<glm::vec2> loadQueueFloat;

	for (auto xz : buildMeshQueue)
	{
		loadQueueFloat.push_back(glm::vec2(xz));
	}

	std::sort(loadQueueFloat.begin(), loadQueueFloat.end(), [p](const glm::vec2& lhs, const glm::vec2& rhs) { return glm::distance(p, lhs) < glm::distance(p, rhs); });

	buildMeshQueue.clear();

	//std::cout << "Sort done" << std::endl;

	for (auto xz : loadQueueFloat)
	{
		buildMeshQueue.push_back(glm::ivec2(xz));
		//std::cout << "(" << xz.x << ", " << xz.y << ")" << std::endl;
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
		log += "R: " + std::to_string(refreshMeshQueue.size()) + " / ";
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
			
			// empty all the queue. 
			preGenerateQueue.clear();
			smoothQueue.clear();
			generateQueue.clear();
			addStructureQueue.clear();
			buildMeshQueue.clear();
			refreshMeshQueue.clear();

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
				while (running && isAllWorkQueueEmpty() && workState.load() == WORK_STATE::RUNNING)
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
				}
				// If there is nothing to generate, start to build mesh
				else if (!buildMeshQueue.empty())
				{
					chunkXZ = buildMeshQueue.front();
					buildMeshQueue.pop_front();
					workType = WorkType::BUILD_MESH;
				}
				else if (!refreshMeshQueue.empty())
				{
					chunkXZ = refreshMeshQueue.front();
					refreshMeshQueue.pop_front();
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
				/**
				*	PRE_GENERATE
				*	Pre-generates chunk. Find out which region that chunk is at in block level. 
				*	Creates region map and store in chunk.
				*	Based on region map, generates height map. Merge with plain height map and stores in chunk.
				*	Then mark chunk as not smoothed. 
				*
				*	Note: PRE_GENERATE pre-generates all chunks whether they are active or not.
				*/
				if (workType == WorkType::PRE_GENERATE)
				{
					//auto start = Utility::Time::now();

					// Get chunk
					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);

					// Chunk is valid most of the time to be honest. However, just in case.
					if (chunk)
					{
						//std::cout << "PreGen " << Utility::Log::vec2ToStr(chunkXZ) << "\n";

						// Check if chunk has already pre generated.. 
						if (chunk->preGenerated.load())
						{
							// Chunk is already pre generated and smoothed. Pass to next step. SMOOTH.
							addSmoothWork(chunkXZ);
						}
						else
						{
							// Chunk has not pre generated and smoothed.

							// Intiailzie random engine for chunk
							chunk->initRandomEngine(world->getSeed());

							// Initialize region map with -1
							std::vector<unsigned int> regionMap(Constant::CHUNK_SECTION_WIDTH * Constant::CHUNK_SECTION_LENGTH, -1);

							// Block size
							const float step = 1.0f;	

							// Get first x and z
							float x = (chunkXZ.x * Constant::CHUNK_BORDER_SIZE);
							float z = (chunkXZ.y * Constant::CHUNK_BORDER_SIZE);

							// region ID look up table
							std::unordered_set<unsigned int> regionIDSet;

							// iterate all blocks in x and z axis
							for (int i = 0; i < Constant::CHUNK_SECTION_WIDTH; i++)
							{
								for (int j = 0; j < Constant::CHUNK_SECTION_LENGTH; j++)
								{
									// get block pos. Add 0.5f because noise returns 0 if input is 0
									glm::vec2 blockXZPos = glm::vec2(x + 0.5f, z + 0.5f);

									// First, check if block is in boundary
									bool inBoundary = world->isPointInBoundary(blockXZPos);

									// init region ID
									unsigned int regionID = -1;

									if (inBoundary)
									{
										// Block is in boundary. Find cloest region to block pos
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
												// found it.
											}
											else
											{
												// Even neighbor regions doesn't have this block. Can't figure out why, assert false it.
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

							// Generate plain height map
							std::vector<std::vector<int>> plainHeightMap;
							HeightMap::generatePlainHeightMapForChunk(chunk->getPosition(), plainHeightMap);

							// Merge height map. Any height map value lower than plain height map will be replaced to plain height map's value.
							chunk->mergeHeightMap(plainHeightMap);

							// Mark chunk as unsmoothed
							chunk->smoothed.store(false);

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

							// Add to SMOOTH work.
							addSmoothWork(chunkXZ);
						}
					}

					//auto end = Utility::Time::now();
					//std::cout << "pg t: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
				}
				/**
				*	SMOOTH
				*	Smooths chunk. If chunk has more than 1 region, there is a high chance that 
				*	chunk needs a interpolation between two different terrain heights.
				*	SMOOTH is part of pre-generation. So all chunks needs to be smoothed.
				*
				*	To smooth the height map, there must be nearby chunks to get nearby heights.
				*	Therefore, SMOOTH skips if chunk is not active. 
				*	Once chunk is smoothed, 
				*/
				else if (workType == WorkType::SMOOTH)
				{
					// There must be a chunk. Chunk loader creates empty chunk.
					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);
					if (chunk)
					{
						// Check if chunk has already smoothed.
						if (chunk->smoothed.load())
						{
							// Chunk has already smoothed height map. Pass to next step. GENERATE.
							addGenerateWork(chunkXZ);
						}
						else
						{
							// Chunk has not smoothed height map yet. Check if it's active
							if (chunk->isActive())
							{
								// check if chunk has multiple regions.
								if (chunk->hasMultipleRegion())
								{
									//std::cout << "Smooth " << Utility::Log::vec2ToStr(chunkXZ) << "\n";

									// Get nearby chunks
									std::vector<std::vector<std::shared_ptr<Chunk>>> nearByChunks = map->getNearByChunks(chunkXZ);

									const int q11 = nearByChunks.at(2).at(2)->getQ22();
									const int q12 = nearByChunks.at(0).at(2)->getQ21();
									const int q21 = nearByChunks.at(2).at(0)->getQ12();
									const int q22 = nearByChunks.at(0).at(0)->getQ11();

									chunk->smoothed.store(true);
									HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 0, 0, Constant::CHUNK_SECTION_WIDTH, Constant::CHUNK_SECTION_LENGTH);

									/*
									// Need correct q11, q12, q21, q22
									HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 0, 0, 8, 8);
									HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 8, 0, 16, 8);
									HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 0, 8, 8, 16);
									HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 8, 8, 16, 16);
									*/
								}
								// Else, chunk has single region. Doesn't have to smooth now.

								if (!map->isChunkOnEdge(chunkXZ))
								{
									// Only generate chunk that is in render distance
									addGenerateWork(chunkXZ);
								}
								// Else, chunk is on out of render distance. Work is done.
							}
							// Else, chunk is not active. work is done.
						}
					}
				}
				/**
				*	GENERATE
				*	Generates chunk. Even though this step is GENERATE, 
				*	it smoothes chunks that is near by chunks with multiple regions.
				*
				*	Once smoothing is done, it generates chunk. 
				*	It fills the block based on height map.
				*/
				else if (workType == WorkType::GENERATE)
				{
					// Get chunk
					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);

					if (chunk)
					{
						// Check if chunk is active once more.
						if (chunk->isActive())
						{
							// Check if chunk is generated
							if (!chunk->isGenerated())
							{
								// Check if chunk is smoothed
								if (!chunk->smoothed.load())
								{
									// Chunk is not smoothed. Check nearby chunk and see if chunk needs to be smoothed
									std::vector<std::vector<std::shared_ptr<Chunk>>> nearByChunks = map->getNearByChunks(chunkXZ);

									// Check if there is a chunk that has mutliple region near by
									bool hasMultiRegionChunk = false;

									auto& EChunk = nearByChunks.at(1).at(0);
									if (EChunk) if (EChunk->hasMultipleRegion()) hasMultiRegionChunk = true;
									auto& WChunk = nearByChunks.at(1).at(2);
									if (WChunk) if (WChunk->hasMultipleRegion()) hasMultiRegionChunk = true;
									auto& SChunk = nearByChunks.at(0).at(1);
									if (SChunk) if (SChunk->hasMultipleRegion()) hasMultiRegionChunk = true;
									auto& NChunk = nearByChunks.at(2).at(1);
									if (NChunk) if (NChunk->hasMultipleRegion()) hasMultiRegionChunk = true;

									// Check if has multi regioned chunk near by
									if (hasMultiRegionChunk)
									{
										// Has it.
										int needSmooth = 0;

										// get chunk's Qs
										const int cq11 = chunk->getQ11();
										const int cq12 = chunk->getQ12();
										const int cq21 = chunk->getQ21();
										const int cq22 = chunk->getQ22();

										// Diff level. 
										const int diff = 3;

										// Check all the near by chunks
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
											const int q11 = nearByChunks.at(2).at(2)->getQ22();
											const int q12 = nearByChunks.at(0).at(2)->getQ21();
											const int q21 = nearByChunks.at(2).at(0)->getQ12();
											const int q22 = nearByChunks.at(0).at(0)->getQ11();

											HeightMap::smoothHelper(chunk->heightMap, q11, q12, q21, q22, 0, 0, Constant::CHUNK_SECTION_WIDTH, Constant::CHUNK_SECTION_LENGTH);

											chunk->smoothed.store(true);
										}
									}
									// Else, there is no multi regioned chunk near by. No need to smooth.
								}
								// Else, chunk has already smoothed

								//std::cout << "Gen " << Utility::Log::vec2ToStr(chunkXZ) << "\n";

								// Generate chunk sections
								chunk->generateChunkSections(2, chunk->findMaxY() / Constant::CHUNK_SECTION_HEIGHT);

								// All chunks starts from chunk section 3 because sea level starts at 33.
								chunk->generate();

								// Pass to next step. ADD_STRUCTURE
								addStructureWork(chunkXZ);

								//auto e = Utility::Time::now();
								//std::cout << "Chunk generation took: " << Utility::Time::toMilliSecondString(s, e) << std::endl;
							}
						}
						// Else, chunk is not active. Do not generate. End of work.
					}
					// Else, chunk is nullptr
				}
				/**
				*	ADD_STRUCTURE
				*	Add structure to chunk. 
				*	If region has any large strucutures that uses multiple chunk, check if has one. Get block schematic and place it.
				*	If chunk has large vegitation to add like trees or boulder, to at end of procedure. 
				*	If chunk is used for large structure, don't spawn trees or boulders.
				*/
				else if (workType == WorkType::ADD_STRUCTURE)
				{
					auto chunk = map->getChunkAtXZ(chunkXZ.x, chunkXZ.y);

					// Chunk must be valid and is active (Just in case)
					if (chunk && chunk->isActive())
					{
						//std::cout << "AddStructure " << Utility::Log::vec2ToStr(chunkXZ) << "\n";
						/*
							Trick of using random.

							During this step, we use unique random generator using chunk position as seed.
							In that way, random generator will be guaranteed to be unique between other chunks
							Also whenever chunk adds structure on the fly, it will always use same random generator
							We are going to use same engine.
							We change dist whenever we need
						*/

						// Get world seed
						auto worldSeed = world->getSeed();
						
						// First, add small structures like grass, flower, etc
						// Then, add mid sized structures like stone, 
						// Then, check if chunk has structure. If so, add structure
						// Then, add trees. Point is, tree is large and can have up to 1 tree per chunk. We don't want to add tree where large structure exists.

						// If chunk on region border, don't spawn tree
						if (chunk->hasSingleRegion())
						{
							// get region id
							auto regionID = chunk->getFirstRegion();
							auto region = world->getRegion(regionID);

							if (region)
							{
								auto biomeType = region->getBiomeType();

								// Check if biome can spawn grasses
								if (biomeType.hasPlants())
								{
									// Biome can spawn plants. Add plants
								}

								// Check if biome can spawn tree
								if (biomeType.hasTree())
								{
									// biome can spawn tree. 
									int treeRand = std::uniform_int_distribution<>(0, 100)(chunk->randomEngine);
									
									// check chance. Only 1 tree per chunk maximum
									int treeChance = biomeType.getTreeSpawnRate();
									if (treeRand < treeChance)
									{
										glm::ivec2 treeLocalPos = HeightMap::getTreePosition(chunk->getPosition());

										//std::cout << "Adding tree" << "Chunk (" << chunkXZ.x << ", " << chunkXZ.y << ")" << std::endl;

										// Don't spawn tree at the edge of chunk. 
										treeLocalPos = glm::clamp(treeLocalPos, 2, 14);

										//auto treeStart = Utility::Time::now();

										int treeY = chunk->heightMap.at(treeLocalPos.x).at(treeLocalPos.y) + 1;

										TreeBuilder::createTree(biomeType.getRandomTreeType(chunk->randomEngine), map, chunkXZ, glm::ivec3(treeLocalPos.x, treeY, treeLocalPos.y), chunk->randomEngine);

										//auto treeEnd = Utility::Time::now();

										//std::cout << "tree t: " << Utility::Time::toMicroSecondString(treeStart, treeEnd) << std::endl;
									}
								}
							}
						}
						
						// Finally, build mesh.
						addBuildMeshWork(chunkXZ, true);

						if (addStructureQueue.empty())
						{
							// Add structure is done. sort build mesh
							sortBuildMeshQueue(map->getCurrentChunkXZ());
							if (firstInitDone.load() == false)
							{
								firstInitDone.store(true);
							}
						}
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
								//std::cout << "Build mesh " << Utility::Log::vec2ToStr(chunkXZ) << "\n";
								// There can be two cases. 
								// 1. Chunk is newly generated and need mesh.
								// 2. Chunk already has mesh but need to refresh
								//auto s = Utility::Time::now();
								meshGenerator->generateChunkMesh(chunk.get(), map);
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
		refreshMeshQueue.clear();
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