#include "ChunkLoader.h"
#include "ChunkMap.h"

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

	// x
	for (int i = 0; i < renderDistance; i++)
	{
		activeChunks.push_back(std::list<Chunk*>());

		// z
		for (int j = 0; j < renderDistance; j++)
		{
			bool hasChunk = map->hasChunkAtXZ(0, 0);
			if (hasChunk)
			{
				activeChunks.back().push_back(map->getChunkAtXZ(0, 0));
			}
			else
			{
				// Request map generate
			}
		}
	}
}

