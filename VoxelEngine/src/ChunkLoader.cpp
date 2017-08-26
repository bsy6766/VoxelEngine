#include "ChunkLoader.h"
#include "ChunkMap.h"
#include "Chunk.h"
#include <ChunkUtil.h>
#include <iostream>
#include <ChunkWorkManager.h>
#include <Camera.h>
#include <Frustum.h>
#include <Utility.h>

using namespace Voxel;

Voxel::ChunkLoader::ChunkLoader()
	: manualChunks(nullptr)
{
}

Voxel::ChunkLoader::~ChunkLoader()
{
	if (manualChunks)
	{
		delete manualChunks;
	}
}

std::vector<glm::vec2> Voxel::ChunkLoader::init(const glm::vec3 & playerPosition, ChunkMap* map, const int renderDistance, const double curTime)
{
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	// Shift by 1 if player is in negative position in XZ axis.
	if (playerPosition.x < 0) chunkX -= 1;
	if (playerPosition.z < 0) chunkZ -= 1;

	currentChunkPos.x = chunkX;
	currentChunkPos.y = chunkZ;
	
	//auto rdFromCenter = renderDistance - 1;

	int minX = chunkX - renderDistance;
	int maxX = chunkX + renderDistance;
	int minZ = chunkZ - renderDistance;
	int maxZ = chunkZ + renderDistance;

	clear();

	// chunk coodinates that need mesh
	std::vector<glm::vec2> chunkCoordinates;
	// Add chunk where player is standing first
	chunkCoordinates.push_back(glm::vec2(chunkX, chunkZ));

	for (int x = minX; x <= maxX; x++)
	{
		activeChunks.push_back(std::list<Chunk*>());

		for (int z = minZ; z <= maxZ; z++)
		{
			//auto coordinate = glm::ivec2(x, z);
			if (map->hasChunkAtXZ(x, z))
			{
				//std::cout << "[ChunkLoader] Loading chunk at (" << x << ", " << z << ")" << std::endl;
				auto chunk = map->getChunkAtXZ(x, z);
				activeChunks.back().push_back(chunk);
				chunk->setActive(true);
				chunk->setVisibility(true);
				chunk->updateTimestamp(curTime);

				// this is initialization. All chunks' mesh need to get generated.
				if (x == chunkX && z == chunkZ)
				{
					continue;
				}

				chunkCoordinates.push_back(glm::vec2(chunk->getCoordinate()));
			}
			else
			{
				// For now, assume chunk loader loads chunks that has already generated
				clear();
				throw std::runtime_error("ChunkLoader tried to iniailize chunk that's hasn't initialized by ChunkMap");
			}
		}
	}

	return chunkCoordinates;
}

bool Voxel::ChunkLoader::update(const glm::vec3 & playerPosition, ChunkMap* map, ChunkWorkManager* workManager, const double curTime)
{
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	// Shift by 1 if player is in negative position in XZ axis.
	if (playerPosition.x < 0) chunkX -= 1;
	if (playerPosition.z < 0) chunkZ -= 1;

	auto newChunkXZ = glm::ivec2(chunkX, chunkZ);

	if (newChunkXZ != currentChunkPos)
	{
		//std::cout << "Player pos (" << playerPosition.x << ", " << playerPosition.z << ")" << std::endl;
		// Player moved to new chunk.
		// normally, player should move more than 1 chunk at a time. Moving more than 1 chunk (16 blocks = 16 meter in scale)
		// means player is cheating or in god mode or whatever. 

		// Check if it's in boundary
		//auto newChunk = map->getChunkAtXZ(chunkX, chunkZ);
		//assert(newChunk != nullptr);
		auto curChunkWorldPos = Voxel::Math::chunkXZToWorldPosition(currentChunkPos);

		// check if player is out of range. If so, proceed.
		float dist = glm::distance(curChunkWorldPos, glm::vec3(playerPosition.x, 0, playerPosition.z));
		//std::cout << "d = " << dist << std::endl;
		bool isNearby = glm::abs(dist) <= Constant::CHUNK_RANGE;

		//bool inBorder = newChunk->isPointInBorder(playerPosition);
		if (isNearby)
		{
			return false;
		}
		
		// Anyway, first we get how far player moved. In chunk distance.
		// Then find which row and col need to be added based on direction player moved.
		// also find which row and col to pop aswell.
		auto d = newChunkXZ - currentChunkPos;
		std::cout << "Player moved to new chunk (" << chunkX << ", " << chunkZ << ") from chunk (" << currentChunkPos.x << ", " << currentChunkPos.y << ")" << std::endl;
		currentChunkPos = newChunkXZ;
		
		// Always modify x first. I think...that.. is.. better....right?

		/*
			2D std::list

											^
											| -x (West)
					List 
						List --------------------------------------
						List --------------------------------------
						List --------------------------------------
	-z (North) <-		List --------------------------------------		-> +z (South)
						List --------------------------------------
						List --------------------------------------
						List --------------------------------------
		
											| +x (east)
											v
		*/

		auto start = Utility::Time::now();

		std::vector<glm::ivec2> chunksToUnload;
		std::vector<glm::ivec2> chunksToLoad;

		if (d.x != 0)
		{
			// Moved in x axis
			if (d.x < 0)
			{
				// Moved to west
				std::cout << "Player moved to west" << std::endl;

				// Unload all the chunk in the back of the active chunk list
				for (auto chunk : activeChunks.back())
				{
					chunk->setActive(false);
					chunk->setVisibility(false);
					auto pos = chunk->getPosition();
					chunksToUnload.push_back(glm::ivec2(pos.x, pos.z));
				}

				// Pop
				activeChunks.pop_back();

				// Get x from first list and -1 for new list to west (negative x)
				int x = activeChunks.front().front()->position.x - 1;
				// Also get first z in list.
				int zStart = activeChunks.back().front()->position.z;

				// Add new list on front with empty chunks
				activeChunks.push_front(std::list<Chunk*>());

				// Add empty chunks
				int lastZ = activeChunks.back().size() + zStart;
				for (int z = zStart; z < lastZ; z++)
				{
					if (!map->hasChunkAtXZ(x, z))
					{
						map->generateEmptyChunk(x, z);
					}
				}

				for (int z = zStart; z < lastZ; z++)
				{
					auto newChunk = map->getChunkAtXZ(x, z);
					newChunk->setActive(true);
					newChunk->setVisibility(true);
					newChunk->updateTimestamp(curTime);
					activeChunks.front().push_back(newChunk);
					auto pos = activeChunks.front().back()->getPosition();
					chunksToLoad.push_back(glm::ivec2(pos.x, pos.z));
				}
			}
			else
			{
				// moved to east
				std::cout << "Player moved to east" << std::endl;

				// Unload all the chunk in the front of chunk list
				for (auto chunk : activeChunks.front())
				{
					chunk->setActive(false);
					chunk->setVisibility(false);
					auto pos = chunk->getPosition();
					chunksToUnload.push_back(glm::ivec2(pos.x, pos.z));
				}

				// Pop
				activeChunks.pop_front();

				// Get x from last list and +1 for new list to east (positive x)
				int x = activeChunks.back().front()->position.x + 1;
				// Also get first z in list.
				int zStart = activeChunks.back().front()->position.z;

				// Add new list on front with empty chunks
				activeChunks.push_back(std::list<Chunk*>());

				// Add empty chunks
				int lastZ = activeChunks.front().size() + zStart;
				for (int z = zStart; z < lastZ; z++)
				{
					if (!map->hasChunkAtXZ(x, z))
					{
						map->generateEmptyChunk(x, z);
					}
				}

				for (int z = zStart; z < lastZ; z++)
				{
					auto newChunk = map->getChunkAtXZ(x, z);
					newChunk->setActive(true);
					newChunk->setVisibility(true);
					newChunk->updateTimestamp(curTime);
					activeChunks.back().push_back(newChunk);
					auto pos = activeChunks.back().back()->getPosition();
					chunksToLoad.push_back(glm::ivec2(pos.x, pos.z));
				}
			}
		}
		// Else, didn't move in X axis

		if (d.y/*z*/ != 0)
		{
			// Moved in z axis
			if (d.y < 0)
			{
				// Move to north
				std::cout << "Player moved to north" << std::endl;

				// Get z
				int z = activeChunks.front().front()->position.z - 1;

				// Iterate through active chunks and unload all the chunks in front of sub list
				for (auto& chunksZ : activeChunks)
				{
					chunksZ.back()->setActive(false);
					chunksZ.back()->setVisibility(false);
					auto pos = chunksZ.back()->getPosition();
					//std::cout << "Deactivating chunk at (" << pos.x << ", " << pos.z << ")" << std::endl;
					chunksToUnload.push_back(glm::ivec2(pos.x, pos.z));
					chunksZ.pop_back();
				}				

				// Iterate through active chunks and add generate new chunk
				for (auto& chunksZ : activeChunks)
				{
					int curX = chunksZ.front()->position.x;
					if (!map->hasChunkAtXZ(curX, z))
					{
						// map doesn't have chunk. create empty
						map->generateEmptyChunk(curX, z);
					}
					// We need to generate chunk first because thread will be able to access new chunk while building a mesh, which results weired wall of mesh 
				}

				for (auto& chunksZ : activeChunks)
				{
					int curX = chunksZ.front()->position.x;

					// get chunk
					auto newChunk = map->getChunkAtXZ(curX, z);
					newChunk->setActive(true);
					newChunk->setVisibility(true);
					newChunk->updateTimestamp(curTime);
					chunksZ.push_front(newChunk);
					auto pos = chunksZ.front()->getPosition();
					//std::cout << "Activating chunk at (" << pos.x << ", " << pos.z << ")" << std::endl;
					chunksToLoad.push_back(glm::ivec2(pos.x, pos.z));
				}
			}
			else
			{
				// Moved to sourth
				std::cout << "Player moved to south" << std::endl;

				// Get z. This is the new z coordinate that we will add to active chunk list
				int z = activeChunks.front().back()->position.z + 1;
				//std::cout << "new z = " << z << std::endl;

				// Iterate through active chunks and unload all the chunks in front of sub list
				for (auto& chunksZ : activeChunks)
				{
					chunksZ.front()->setActive(false);
					chunksZ.front()->setVisibility(false);
					auto pos = chunksZ.front()->getPosition();
					//std::cout << "Deactivating chunk at (" << pos.x << ", " << pos.z << ")" << std::endl;
					chunksToUnload.push_back(glm::ivec2(pos.x, pos.z));
					chunksZ.pop_front();
				}				

				// Iterate through active chunks and add generate new chunk.
				for (auto& chunksZ : activeChunks)
				{
					int curX = chunksZ.front()->position.x;
					if (!map->hasChunkAtXZ(curX, z))
					{
						// map doesn't have chunk. create empty
						map->generateEmptyChunk(curX, z);
					}
					// We need to generate chunk first because thread will be able to access new chunk while building a mesh, which results weired wall of mesh 
				}

				for (auto& chunksZ : activeChunks)
				{
					int curX = chunksZ.front()->position.x;

					// get chunk
					auto newChunk = map->getChunkAtXZ(curX, z);
					newChunk->setActive(true);
					newChunk->setVisibility(true);
					newChunk->updateTimestamp(curTime);
					chunksZ.push_back(newChunk);
					auto pos = chunksZ.back()->getPosition();
					//std::cout << "Activating chunk at (" << pos.x << ", " << pos.z << ")" << std::endl;
					chunksToLoad.push_back(glm::ivec2(pos.x, pos.z));
				}
			}
		}
		// Else, didn't move in Z axis

		bool notify = false;
		if (!chunksToUnload.empty())
		{
			workManager->addUnload(chunksToUnload);
			notify = true;
		}

		if (notify)
		{
			workManager->notify();
		}

		if (!chunksToLoad.empty())
		{
			workManager->addLoad(chunksToLoad);
			notify = true;
		}

		if (notify)
		{
			workManager->notify();
		}

		auto end = Utility::Time::now();
		std::cout << "Chunk loader update took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;

		return true;
	}

	return false;
}

int Voxel::ChunkLoader::findVisibleChunk()
{
	int count = 0;
	for (auto x : activeChunks)
	{
		for (auto chunk : x)
		{
			if (chunk != nullptr)
			{
				if (glm::ivec2(chunk->position.x, chunk->position.z) == currentChunkPos)
				{
					// make current chunk always visible
					chunk->setVisibility(true);
					count++;
				}
				else
				{
					bool visible = Camera::mainCamera->getFrustum()->isChunkBorderInFrustum(chunk);
					chunk->setVisibility(visible);
					if (visible) count++;
				}
			}
		}
	}

	return count;
}

int Voxel::ChunkLoader::getActiveChunksCount()
{
	return activeChunks.size() * activeChunks.front().size();
}

void Voxel::ChunkLoader::render()
{
	//auto start = Utility::Time::now();
	for (auto x : activeChunks)
	{
		for (auto chunk : x)
		{
			if (chunk != nullptr)
			{
				if (chunk->isGenerated() && chunk->isVisible())
				{
					chunk->render();
				}
			}
		}
	}
	//auto end = Utility::Time::now();
	//std::cout << " t = " << Utility::Time::toMicroSecondString(start, end) << std::endl;
}


void ChunkLoader::clear()
{
	for (auto x : activeChunks)
	{
		for (auto z : x)
		{
			if (z != nullptr)
			{
				z->setActive(false);
			}
		}
	}

	activeChunks.clear();
}

