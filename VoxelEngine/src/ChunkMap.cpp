#include "ChunkMap.h"
#include <Chunk.h>
#include <Block.h>
#include <ChunkSection.h>
#include <iostream>
#include <Utility.h>

using namespace Voxel;

Voxel::ChunkMap::~ChunkMap()
{
	clear();
}

void Voxel::ChunkMap::initSpawnChunk()
{
	//std::cout << "[ChunkMap] Creating map..." << std::endl;
	
	//std::cout << "[ChunkMap] Initializing spawn chunks.." << std::endl;
	
	// spawn chunk is always loaded.
	//glm::ivec2 spawnChunkMinPos = glm::ivec2(-SPAWN_CHUNK_DISTANCE);
	//glm::ivec2 spawnChunkMaxPos = glm::ivec2(SPAWN_CHUNK_DISTANCE);

	int spawnX = -(Constant::SPAWN_CHUNK_DISTANCE - 1);
	int spawnMaxX = (Constant::SPAWN_CHUNK_DISTANCE * 2) - 1 + spawnX;
	int spawnMaxZ = spawnMaxX;
	int spawnZ = spawnX;

	// add spawn chunk. Todo: If this isn't a first initialization, read from map file
	for (int x = spawnX; x < spawnMaxX; x++)
	{
		for (int z = spawnZ; z < spawnMaxZ; z++)
		{
			//std::cout << "[ChunkMap] Adding (" << x << ", " << z << ") as spawn chunk." << std::endl;

			// Add to LUT
			chunkLUT.emplace(glm::ivec2(x, z));

			// Add new spawn chunk
			Chunk* newChunk = Chunk::create(x, z);
			map.emplace(glm::ivec2(x, z), newChunk);
		}
	}

	// Then add chunks near player.
	// Todo: get render distance from setting
	int renderDistnace = 4;

}

void Voxel::ChunkMap::initChunkNearPlayer(const glm::vec3 & playerPosition, const int renderDistance)
{
	// Get chunk X and Z where player is standing
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	if (playerPosition.x < 0) chunkX -= 1;
	if (playerPosition.z < 0) chunkZ -= 1;

	//std::cout << "[ChunkMap] Initializing chunk near player at (" << chunkX << ", " << chunkZ << ")" << std::endl;

	// excluding the chunk where player stands
	//auto rdFromCenter = renderDistance - 1;

	int minX = chunkX - renderDistance;
	int maxX = chunkX + renderDistance;
	int minZ = chunkZ - renderDistance;
	int maxZ = chunkZ + renderDistance;

	for (int x = minX; x <= maxX; x++)
	{
		for (int z = minZ; z <= maxZ; z++)
		{
			auto coordinate = glm::ivec2(x, z);
			if (chunkLUT.find(coordinate) == chunkLUT.end())
			{
				// new chunk
				//std::cout << "[ChunkMap] Adding (" << x << ", " << z << ") chunk." << std::endl;
				Chunk* newChunk = Chunk::create(x, z);
				map.emplace(coordinate, newChunk);

				// Add to LUt
				chunkLUT.emplace(coordinate);
			}
			else
			{
				// Chunk is already loaded on map
				continue;
			}
		}
	}
}

void ChunkMap::clear()
{
	for (auto e : map)
	{
		if (e.second)
		{
			delete e.second;
		}
	}

	map.clear();
}

bool Voxel::ChunkMap::hasChunkAtXZ(int x, int z)
{
	std::unique_lock<std::mutex> lock(mapMutex);
	auto v2 = glm::ivec2(x, z);
	return chunkLUT.find(v2) != chunkLUT.end();
}

Chunk * Voxel::ChunkMap::getChunkAtXZ(int x, int z)
{
	std::unique_lock<std::mutex> lock(mapMutex);
	auto find_it = map.find(glm::ivec2(x, z));
	if (find_it == map.end())
	{
		return nullptr;
	}
	else
	{
		return find_it->second;
	}
}

void Voxel::ChunkMap::generateRegion(const glm::ivec2& regionCoordinate)
{
	int chunkX = regionCoordinate.x * Constant::REGION_WIDTH;
	int chunkZ = regionCoordinate.y * Constant::REGION_LENGTH;

	int chunkXLen = chunkX + Constant::REGION_WIDTH;
	int chunkZLen = chunkZ + Constant::REGION_LENGTH;

	for (int x = chunkX; x < chunkXLen; x++)
	{
		for (int z = chunkZ; z < chunkZLen; z++)
		{
			generateChunk(x, z);
			//generateEmptyChunk(x, z);
		}
	}
}

void Voxel::ChunkMap::generateChunk(const int x, const int z)
{
	// for sake, just check one more time
	if (!hasChunkAtXZ(x, z))
	{
		std::unique_lock<std::mutex> lock(mapMutex);
		Chunk* newChunk = Chunk::create(x, z);
		map.emplace(glm::ivec2(x, z), newChunk);

		// Add to LUt
		chunkLUT.emplace(glm::ivec2(x, z));
	}
}

void Voxel::ChunkMap::generateEmptyChunk(const int x, const int z)
{
	// for sake, just check one more time
	if (!hasChunkAtXZ(x, z))
	{
		std::unique_lock<std::mutex> lock(mapMutex);
		Chunk* newChunk = Chunk::createEmpty(x, z);
		map.emplace(glm::ivec2(x, z), newChunk);

		// Add to LUt
		chunkLUT.emplace(glm::ivec2(x, z));
	}
}

unsigned int Voxel::ChunkMap::getSize()
{
	return map.size();
}

Block * Voxel::ChunkMap::getBlockAtWorldXYZ(int x, int y, int z)
{
	int chunkX = x / Constant::CHUNK_SECTION_WIDTH;
	int localX = x % Constant::CHUNK_SECTION_WIDTH;

	if (localX < 0)
	{
		localX += Constant::CHUNK_SECTION_WIDTH;
		chunkX -= 1;
	}
	else if (localX >= Constant::CHUNK_SECTION_WIDTH)
	{
		localX -= Constant::CHUNK_SECTION_WIDTH;
		chunkX += 1;
	}

	int chunkY = y / Constant::CHUNK_SECTION_HEIGHT;
	int localY = y % Constant::CHUNK_SECTION_HEIGHT;

	if (chunkY == 0)
	{
		if (localY < 0)
		{
			// There is no chunk lower thak 0 
			return nullptr;
		}
		else if (localY >= Constant::CHUNK_SECTION_HEIGHT)
		{
			localY -= Constant::CHUNK_SECTION_HEIGHT;
			chunkY += 1;
		}
	}
	else
	{
		if (localY < 0)
		{
			localY += Constant::CHUNK_SECTION_HEIGHT;
			chunkY -= 1;
		}
		else if (localY >= Constant::CHUNK_SECTION_HEIGHT)
		{
			localY -= Constant::CHUNK_SECTION_HEIGHT;
			chunkY += 1;
		}
	}

	int chunkZ = z / Constant::CHUNK_SECTION_LENGTH;
	int localZ = z % Constant::CHUNK_SECTION_LENGTH;

	if (localZ < 0)
	{
		localZ += Constant::CHUNK_SECTION_LENGTH;
		chunkZ -= 1;
	}
	else if (localZ >= Constant::CHUNK_SECTION_LENGTH)
	{
		localZ -= Constant::CHUNK_SECTION_LENGTH;
		chunkZ += 1;
	}

	bool hasChunk = this->hasChunkAtXZ(chunkX, chunkZ);

	if (!hasChunk)
	{
		// There is no chunk generated. 
		return nullptr;
	}
	else
	{
		// target chunk
		auto chunk = this->getChunkAtXZ(chunkX, chunkZ);
		if (chunk)
		{
			if (chunk->isActive())
			{
				// target chunk section
				auto chunkSection = chunk->getChunkSectionByY(chunkY);
				if (chunkSection)
				{
					// return block
					return chunkSection->getBlockAt(localX, localY, localZ);
				}
				// There is no block in this chunk section = nullptr
			}
			// Can't access block that is in inactive chunk
		}
	}

	return nullptr;
}

Block* Voxel::ChunkMap::raycastBlock(const glm::vec3& playerPosition, const glm::vec3& playerDirection, const float playerRange)
{
	//std::cout << "RayCasting" << std::endl;

	//std::cout << "player pos = " << playerPosition.x << ", " << playerPosition.y << ", " << playerPosition.z << ")" << std::endl;
	//std::cout << "player dir = " << playerDirection.x << ", " << playerDirection.y << ", " << playerDirection.z << ")" << std::endl;

	auto rayStart = playerPosition;
	auto rayEnd = playerPosition + (playerDirection * playerRange);

	//std::cout << "rayStart = " << rayStart.x << ", " << rayStart.y << ", " << rayStart.z << ")" << std::endl;
	//std::cout << "rayEnd = " << rayEnd.x << ", " << rayEnd.y << ", " << rayEnd.z << ")" << std::endl;

	glm::vec3 dirVec = rayEnd - rayStart;

	float div = 200.0f;
	glm::vec3 step = dirVec / div;

	//std::cout << "step = " << step.x << ", " << step.y << ", " << step.z << ")" << std::endl;

	int threshold = 200;

	glm::vec3 curRayPoint = rayStart;

	glm::ivec3 startBlockPos = glm::ivec3(Utility::Math::fastFloor(rayStart.x), Utility::Math::fastFloor(rayStart.y), Utility::Math::fastFloor(rayStart.z));
	glm::ivec3 curBlockPos = startBlockPos;

	//std::cout << "start block (" << startBlockPos.x << ", " << startBlockPos.y << ", " << startBlockPos.z << ")" << std::endl;

	while (threshold >= 0)
	{
		curRayPoint += step;

		if (glm::distance(curRayPoint, rayStart) > playerRange)
		{
			return nullptr;
		}
		//std::cout << "visiting (" << curRayPoint.x << ", " << curRayPoint.y << ", " << curRayPoint.z << ")" << std::endl;

		auto visitingBlockPos = glm::ivec3(Utility::Math::fastFloor(curRayPoint.x), Utility::Math::fastFloor(curRayPoint.y), Utility::Math::fastFloor(curRayPoint.z));

		if (visitingBlockPos != curBlockPos)
		{
			curBlockPos = visitingBlockPos;

			//std::cout << "cur block (" << curBlockPos.x << ", " << curBlockPos.y << ", " << curBlockPos.z << ")" << std::endl;
			Block* curBlock = getBlockAtWorldXYZ(curBlockPos.x, curBlockPos.y, curBlockPos.z);

			if (curBlock)
			{
				if (curBlock->isEmpty() == false)
				{
					// raycasted block not empty. 
					if (curBlockPos != startBlockPos)
					{
						//std::cout << "Block hit (" << curBlockPos.x << ", " << curBlockPos.y << ", " << curBlockPos.z << ")" << std::endl;
						return curBlock;
					}
				}
			}
		}


		threshold--;
	}

	return nullptr;
}

void Voxel::ChunkMap::releaseChunk(const glm::ivec2 & coordinate)
{
	if (hasChunkAtXZ(coordinate.x, coordinate.y))
	{
		auto chunk = getChunkAtXZ(coordinate.x, coordinate.y);
		if (chunk)
		{
			std::unique_lock<std::mutex> lock(mapMutex);
			chunk->releaseMesh();
			delete chunk;

			map.erase(coordinate);
			chunkLUT.erase(coordinate);

			//std::cout << "Removing chunk (" << coordinate.x << ", " << coordinate.y << ")" << std::endl;
		}
	}
}
