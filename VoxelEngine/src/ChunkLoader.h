#ifndef CHUNK_LOADER_H
#define CHUNK_LOADER_H

#include <glm\glm.hpp>
#include <list>
#include <unordered_map>
#include <shared_ptr.hpp>

namespace Voxel
{
	// Forward everything!
	class Chunk;
	class ChunkMap;
	class ChunkWorkManager;

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
		
		// Chunks that are currently active
		std::list<std::list<std::shared_ptr<Chunk>>> activeChunks;

		void moveWest(ChunkMap* map, std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime);
		void moveEast(ChunkMap* map, std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime);
		void moveSouth(ChunkMap* map, std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime);
		void moveNorth(ChunkMap* map, std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime);
	public:
		ChunkLoader();
		~ChunkLoader();

		// Initialize and generate active chunk list based on player position and render distance.
		// Returns chunk coordinates that need mesh to generate.
		std::vector<glm::vec2> init(const glm::vec3& playerPosition, ChunkMap* map, const int renderDistance, const double curTime);

		// Check if player moved to another chunk. If so, return true.
		bool update(const glm::vec3& playerPosition, ChunkMap* map, ChunkWorkManager* workManager, const double curTime);

		// find visible chunk. Returns the number of chunk that is visible
		int findVisibleChunk();

		// Get size
		int getActiveChunksCount();

		// Render active chunks
		void render();

		// Clears loaded chunk and make deactive.
		void clear();
	};
}

#endif