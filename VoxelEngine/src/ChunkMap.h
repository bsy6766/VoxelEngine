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

	public:
		ChunkMap() = default;
		~ChunkMap();

		// Initialize spawn chunk
		void initSpawnChunk();

		// Initialize map data near by player based on player's last position and render distance
		void initChunkNearPlayer(const glm::vec3& playerPosition, const int renderDistance);

		// Clears all the chunk in the map
		void clear();

		// Check if has chunk in x and z coordinate
		bool hasChunkAtXZ(int x, int z);

		// Get chunk in x and z coordinate
		Chunk* getChunkAtXZ(int x, int z);

		// Generate region based on coordinate and size.
		void generateRegion(const glm::ivec2& regionCoordinate);

		// Generate chunk with specific coordinate
		void generateChunk(const int x, const int z);

		// Generate empty chunk. 
		void generateEmptyChunk(const int x, const int z);

		// Get size of map
		unsigned int getSize();

		// Get block in world position. Returns nullptr if chunk, chunksection or block doesn't exsits.
		// boo valid tells wether block exists or not. It will be true if block exists. However, return value can still be nullptr if it's air block. False if chunk or chunk section doesn't exists = no block.
		Block* getBlockAtWorldXYZ(int x, int y, int z, bool& valid);
		
		// From rayStart to rayEnd, visit all blocks
		Block* raycastBlock(const glm::vec3& playerPosition, const glm::vec3& playerDirection, const float playerRange);

		// Release and delete chunk 
		void releaseChunk(const glm::ivec2& coordinate);
	};
}


#endif