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
		// A chunk position currently player is standing
		glm::ivec2 currentChunkPos;
		
		// Chunks that are manually added to be active.
		ChunkMap* manualChunks;
		// Chunks that are currently active
		std::list<std::list<Chunk*>> activeChunks;
	public:
		ChunkLoader();
		~ChunkLoader();

		// Initialize and generate active chunk list based on player position and render distance.
		// Returns chunk coordinates that need mesh to generate.
		std::vector<glm::vec2> init(const glm::vec3& playerPosition, ChunkMap* map, const int renderDistance);

		// Check if player moved to another chunk. If so, return true.
		bool update(const glm::vec3& playerPosition, ChunkMap* map, glm::ivec2& mod);
		// find visible chunk
		void findVisibleChunk();

		// Render active chunks
		void render();

		// Clears loaded chunk and make deactive.
		void clear();
	};
}

#endif