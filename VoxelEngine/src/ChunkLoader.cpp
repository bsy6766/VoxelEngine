#include "ChunkLoader.h"
#include "ChunkMap.h"
#include "Chunk.h"
#include <ChunkUtil.h>
#include <iostream>

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

void Voxel::ChunkLoader::init(const glm::vec3 & playerPosition, ChunkMap* map, const int renderDistance)
{
	/*
	// Temp. testing multiple chunks
	activeChunks.push_back(std::list<Chunk*>());
	auto chunk00 = map->getChunkAtXZ(0, 0);
	chunk00->setActive(true);
	activeChunks.back().push_back(chunk00);
	
	auto chunk01 = map->getChunkAtXZ(0, 1);
	chunk01->setActive(true);
	activeChunks.back().push_back(chunk01);

	activeChunks.push_back(std::list<Chunk*>());

	auto chunk10 = map->getChunkAtXZ(1, 0);
	chunk10->setActive(true);
	activeChunks.back().push_back(chunk10);
	//activeChunks.back().push_back(map->getChunkAtXZ(1, 1));

	*/
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	currentChunkPos.x = chunkX;
	currentChunkPos.y = chunkZ;
	
	auto rdFromCenter = renderDistance - 1;

	int minX = chunkX - rdFromCenter;
	int maxX = chunkX + rdFromCenter;
	int minZ = chunkZ - rdFromCenter;
	int maxZ = chunkZ + rdFromCenter;

	clear();

	for (int x = minX; x <= maxX; x++)
	{
		activeChunks.push_back(std::list<Chunk*>());

		for (int z = minZ; z <= maxZ; z++)
		{
			//auto coordinate = glm::ivec2(x, z);
			if (map->hasChunkAtXZ(x, z))
			{
				//std::cout << "[ChunkLoader] Loading chunk at (" << x << ", " << z << ")" << std::endl;
				activeChunks.back().push_back(map->getChunkAtXZ(x, z));
				activeChunks.back().back()->setActive(true);
			}
			else
			{
				// For now, assume chunk loader loads same chunk as chunk map initailized
				clear();
				throw std::runtime_error("ChunkLoader tried to iniailize chunk that's hasn't initialized by ChunkMap");
			}
		}
	}
}

bool Voxel::ChunkLoader::updatePlayerPosition(const glm::vec3 & playerPosition, ChunkMap* map)
{
	int chunkX = static_cast<int>(playerPosition.x) / Constant::CHUNK_SECTION_WIDTH;
	int chunkZ = static_cast<int>(playerPosition.z) / Constant::CHUNK_SECTION_LENGTH;

	auto newChunkXZ = glm::ivec2(chunkX, chunkZ);

	if (newChunkXZ != currentChunkPos)
	{
		// Player moved to new chunk.
		// normally, player should move more than 1 chunk at a time. Moving more than 1 chunk (16 blocks = 16 meter in scale)
		// means player is cheating or in god mode or whatever. 
		
		// Anyway, first we get how far player moved. In chunk distance.
		// Then find which row and col need to be added based on direction player moved.
		// also find which row and col to pop aswell.
		auto d = newChunkXZ - currentChunkPos;
		std::cout << "Player moved to new chunk (" << chunkX << ", " << chunkZ << ") from chunk (" << currentChunkPos.x << ", " << currentChunkPos.y << ")" << std::endl;
		currentChunkPos = newChunkXZ;

		return true;
	}

	return false;
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

