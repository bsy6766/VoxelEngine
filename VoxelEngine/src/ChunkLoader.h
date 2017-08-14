#ifndef CHUNK_LOADER_H
#define CHUNK_LOADER_H

#include <glm\glm.hpp>
#include <list>
#include <unordered_map>

namespace Voxel
{
	// Forward everything!
	class Chunk;
	class ChunkMap;

	/**
	*	@class ChunkLoader
	*	@brief Loads and manages visible chunks
	*/
	class ChunkLoader
	{
		friend class ChunkMeshGenerator;
	private:
		// A chunk where currently player is standing
		Chunk* currentChunk;
		// A chunk that needs to be current chunk
		Chunk* nextChunk;

		// Chunks that are manually added to be active.
		ChunkMap* manualChunks;
		// Chunks that are currently active
		std::list<std::list<Chunk*>> activeChunks;
	public:
		ChunkLoader();
		~ChunkLoader();

		// Initialize and generate active chunk list based on player position and render distance
		void init(const glm::vec3& playerPosition, ChunkMap* map, const int renderDistance);

		// Check if player moved to another chunk. If so, return true.
		bool updatePlayerPosition(const glm::vec3& position);
		// update active chunk
	};
}

#endif