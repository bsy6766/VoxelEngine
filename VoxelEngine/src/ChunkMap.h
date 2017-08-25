#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

#include <glm\glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <ChunkUtil.h>
#include <mutex>

namespace Voxel
{
	// forward
	class Chunk;
	class Block;

	typedef std::unordered_map<glm::ivec2, Chunk*, KeyFuncs, KeyFuncs> ChunkUnorderedMap;

	/**
	*	@class ChunkMap
	*	@brief Simple wrapper or unordered_map with Chunk with some functionalities.
	*
	*	Chunk map stores all the chuncks that are ever generated.
	*	It keep tracks if chunk has been explored and generated or not.
	*	Chunk map doesn't update chunk. It only keeps the data.
	*
	*	Chunk map always loads region (0, 0), which means 32 x 32 chunks
	*/
	class ChunkMap
	{
	private:
		// chunk map
		ChunkUnorderedMap map;
		// mutex for modifying map;
		std::mutex mapMutex;

		// Chunk LUT. This stores chunk position (x, y. Not world pos) that has been ever generated
		std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs> chunkLUT;
		// Active Chunk LUT. This stores chunk position that is currently loaded in game
		//std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs> activeChunkLUT;

	public:
		ChunkMap() = default;
		~ChunkMap();

		// Initialize spawn chunk
		void initSpawnChunk();
		// Initialize map data near by player based on player's last position and render distance
		void initChunkNearPlayer(const glm::vec3& playerPosition, const int renderDistance);

		// Clears all the chunk in the map
		void clear();

		ChunkUnorderedMap& getMapRef();

		bool hasChunkAtXZ(int x, int z);
		Chunk* getChunkAtXZ(int x, int z);

		void generateRegion(const glm::ivec2& regionCoordinate);
		void generateChunk(const int x, const int z);
		void generateEmptyChunk(const int x, const int z);
		unsigned int getSize();

		Block* getBlockAtWorldXYZ(int x, int y, int z);

		// Attemp to laod chunk in map. If map already has chunk, 
		bool attempChunkLoad(int x, int z);

		// From rayStart to rayEnd, visit all blocks
		//void raycast(const glm::vec3& rayStart, const glm::vec3& rayEnd);
		Block* raycastBlock(const glm::vec3& playerPosition, const glm::vec3& playerDirection, const float playerRange);

		void moveChunkToUnloadMap(const glm::ivec2& coordinate);
		void releaseChunk(const glm::ivec2& coordinate);
	};
}


#endif