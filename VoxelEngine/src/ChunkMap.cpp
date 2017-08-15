#include "ChunkMap.h"
#include <Chunk.h>
#include <ChunkSection.h>
#include <ChunkUtil.h>
#include <iostream>

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

	/*
	Chunk* newChunk = Chunk::create(0, 0);
	map.emplace(glm::ivec2(0, 0), newChunk);
	Chunk* newChunk1 = Chunk::create(0, 1);
	map.emplace(glm::ivec2(0, 1), newChunk1);
	Chunk* newChunk2 = Chunk::create(1, 0);
	map.emplace(glm::ivec2(1, 0), newChunk2);
	Chunk* newChunk3 = Chunk::create(1, 1);
	map.emplace(glm::ivec2(1, 1), newChunk3);

	return;
	*/

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

	//std::cout << "[ChunkMap] Initializing chunk near player at (" << chunkX << ", " << chunkZ << ")" << std::endl;

	// excluding the chunk where player stands
	auto rdFromCenter = renderDistance - 1;

	int minX = chunkX - rdFromCenter;
	int maxX = chunkX + rdFromCenter;
	int minZ = chunkZ - rdFromCenter;
	int maxZ = chunkZ + rdFromCenter;

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

ChunkUnorderedMap & Voxel::ChunkMap::getMapRef()
{
	return map;
}

bool Voxel::ChunkMap::hasChunkAtXZ(int x, int z)
{
	auto v2 = glm::ivec2(x, z);
	return chunkLUT.find(v2) != chunkLUT.end();
}

Chunk * Voxel::ChunkMap::getChunkAtXZ(int x, int z)
{
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

	auto find_it = map.find(glm::ivec2(chunkX, chunkZ));
	if (find_it == map.end())
	{
		// Chunk hasn't generated yet. 
		// Todo: generate new chunk? for block query that is out of render distance?
		return nullptr;
	}
	else
	{
		// target chunk
		auto chunk = find_it->second;
		if (chunk->isActive())
		{
			// target chunk section
			auto chunkSection = chunk->getChunkSectionByY(chunkY);
			if (chunkSection)
			{
				// return block
				return chunkSection->getBlockAt(localX, localY, localZ);
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			// Can't access block that is in inactive chunk
			return nullptr;
		}
	}
}

bool Voxel::ChunkMap::attempChunkLoad(int x, int z)
{
	return false;
}

void Voxel::ChunkMap::render()
{
	for (auto chunk : map)
	{
		(chunk.second)->render();
	}
}
