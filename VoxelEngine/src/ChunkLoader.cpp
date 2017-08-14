#include "ChunkLoader.h"
#include "ChunkMap.h"
#include "Chunk.h"

using namespace Voxel;

Voxel::ChunkLoader::ChunkLoader()
	: currentChunk(nullptr)
	, nextChunk(nullptr)
	, manualChunks(nullptr)
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
	// Get min chunk position.

	/**/
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

	/*
	// x
	for (int x = 0; x < renderDistance; x++)
	{
		activeChunks.push_back(std::list<Chunk*>());

		// z
		for (int z = 0; z < renderDistance; z++)
		{
			bool hasChunk = map->hasChunkAtXZ(x, z);
			if (hasChunk)
			{
				activeChunks.back().push_back(map->getChunkAtXZ(x, z));
			}
			else
			{
				// Request map generate
				map->attempChunkLoad(x, z);
			}
		}
	}
	*/
}

