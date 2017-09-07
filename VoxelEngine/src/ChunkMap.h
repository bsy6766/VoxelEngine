#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

#include <glm\glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <ChunkUtil.h>
#include <Block.h>
#include <mutex>
#include <Cube.h>
#include <shared_ptr.hpp>

#include <boost\polygon\voronoi.hpp>

namespace Voxel
{
	// forward
	class Chunk;
	class ChunkWorkManager;

	// Raycast result
	struct RayResult
	{
		// Nullptr if ray didn't hit
		Block* block;
		// The face that ray hit
		Cube::Face face;
	};

	typedef std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>, KeyFuncs, KeyFuncs> ChunkUnorderedMap;

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

		// Voronoi 
		void initVoronoi();

		// Initialize spawn chunk
		void initSpawnChunk();

		// Initialize map data near by player based on player's last position and render distance
		void initChunkNearPlayer(const glm::vec3& playerPosition, const int renderDistance);

		// Clears all the chunk in the map
		void clear();

		// Check if has chunk in x and z coordinate
		bool hasChunkAtXZ(int x, int z);

		// Get chunk in x and z coordinate
		//Chunk* getChunkAtXZ(int x, int z);
		std::shared_ptr<Chunk> getChunkAtXZ(int x, int z);

		// Generate region based on coordinate and size.
		void generateRegion(const glm::ivec2& regionCoordinate);

		// Generate chunk with specific coordinate
		void generateChunk(const int x, const int z);

		// Generate empty chunk. 
		void generateEmptyChunk(const int x, const int z);

		// Get size of map
		unsigned int getSize();

		// Convert block world Position to local position and chunk position
		void blockWorldCoordinateToLocalAndChunkSectionCoordinate(const glm::ivec3& blockWorldCoordinate, glm::ivec3& blockLocalCoordinate, glm::ivec3& chunkSectionCoordinate);

		// Get chunks that touches the block
		std::vector<glm::ivec2> getChunksNearByBlock(const glm::ivec3& blockLocalPos, const glm::ivec3& blockChunkPos);

		// Get block in world position. Returns nullptr if chunk, chunksection or block doesn't exsits.
		// boo valid tells wether block exists or not. It will be true if block exists. However, return value can still be nullptr if it's air block. False if chunk or chunk section doesn't exists = no block.
		Block* getBlockAtWorldXYZ(int x, int y, int z);
		
		// Check if block is opaque.
		// Retruns 0 if block exists and transparent. 
		// Returns 1 if block exists and opaque
		// Retruns 2 if chunk section doesn't exists
		// Retruns 3 if chunk doesn't exsits.
		int isBlockAtWorldXYZOpaque(const int x, const int y, const int z);

		// place block at face direction
		void placeBlockAt(const glm::ivec3& blockPos, const Cube::Face& faceDir, ChunkWorkManager* workManager);
		void removeBlockAt(const glm::ivec3& blockPos, ChunkWorkManager* workManager);
		
		// From rayStart to rayEnd, visit all blocks
		RayResult raycastBlock(const glm::vec3& playerPosition, const glm::vec3& playerDirection, const float playerRange);
		// Check which face of block(cube) did ray hit
		Cube::Face raycastFace(const glm::vec3& rayStart, const glm::vec3& rayEnd, const AABB& blockAABB);
		int raycastTriangle(const glm::vec3& rayStart, const glm::vec3& rayEnd, const Triangle& tri, glm::vec3& intersectingPoint);

		// Release and delete chunk 
		void releaseChunk(const glm::ivec2& coordinate);
	};
}


#endif