#include "ChunkMap.h"
#include <Chunk.h>
#include <ChunkSection.h>

using namespace Voxel;

Voxel::ChunkMap::~ChunkMap()
{
	clear();
}

void Voxel::ChunkMap::init(const glm::vec3 & playerPosition)
{
	// Todo: load chunk based on player position
	
	// For now, just load 1 chunk for sake
	chunkLUT.emplace(glm::ivec2(0, 0));
	
	Chunk* newChunk = Chunk::create(0, 0);
	map.emplace(glm::ivec2(0, 0), newChunk);
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
	int chunkX = x / ChunkSection::CHUNK_SECTION_WIDTH;
	int localX = x % ChunkSection::CHUNK_SECTION_WIDTH;

	if (localX < 0)
	{
		localX += 16;
		chunkX -= 1;
	}
	else if (localX > 15)
	{
		localX -= 16;
		chunkX += 1;
	}

	int chunkY = y / ChunkSection::CHUNK_SECTION_HEIGHT;
	int localY = y % ChunkSection::CHUNK_SECTION_HEIGHT;

	if (chunkY == 0)
	{
		if (localY < 0)
		{
			// There is no chunk lower thak 0 
			return nullptr;
		}
		else if (localY > 15)
		{
			localY -= 16;
			chunkY += 1;
		}
	}
	else
	{
		if (localY < 0)
		{
			localY += 16;
			chunkY -= 1;
		}
		else if (localY > 15)
		{
			localY -= 16;
			chunkY += 1;
		}
	}

	int chunkZ = z / ChunkSection::CHUNK_SECTION_LENGTH;
	int localZ = z % ChunkSection::CHUNK_SECTION_LENGTH;

	if (localZ < 0)
	{
		localZ += 16;
		chunkZ -= 1;
	}
	else if (localZ > 15)
	{
		localZ -= 16;
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
}

void Voxel::ChunkMap::render()
{
	for (auto chunk : map)
	{
		(chunk.second)->render();
	}
}
