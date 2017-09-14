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

namespace Voxel
{
	// forward
	class Chunk;
	class ChunkWorkManager;
	class Region;

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
	*	@brief Manages all chunks
	*
	*	ChunkMap manages all the chunks that are in the game. 
	*	It can remove chunk, generate chunk, generate empty chunk, find visible chunk, etc
	*	Also keep tracks the active chunks, this was originally ChunkLoader's job, but merge into ChunkMap
	*
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

		// A chunk position currently player is standing
		glm::ivec2 currentChunkPos;

		// 2D list that keep tracks the active chunk coordinates
		std::list<std::list<glm::ivec2>> activeChunks;

		// Move calls when player moves to new chunk
		void moveWest(std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime);
		void moveEast(std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime);
		void moveSouth(std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime);
		void moveNorth(std::vector<glm::ivec2>& chunksToUnload, std::vector<glm::ivec2>& chunksToLoad, std::vector<glm::ivec2>& chunksToReload, const double curTime);
	public:
		ChunkMap();
		~ChunkMap();

		// Initialize map data near by player based on player's last position and render distance
		void initChunkNearPlayer(const glm::vec3& playerPosition, const int renderDistance);

		// Initialize active chunks 
		std::vector<glm::vec2> initActiveChunks(const int renderDistance);


		// Clears all the chunk in the map
		void clear();

		// Check if has chunk in x and z coordinate
		bool hasChunkAtXZ(int x, int z);

		// Get chunk in x and z coordinate
		//Chunk* getChunkAtXZ(int x, int z);
		std::shared_ptr<Chunk> getChunkAtXZ(int x, int z);

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

		// Get number of active chunks
		int getActiveChunksCount();

		// Update chunk map
		bool update(const glm::vec3& playerPosition, ChunkWorkManager* workManager, const double time);
		
		// find visible chunks. retrusn the number of chunks that is visible
		int findVisibleChunk();

		// render chunks
		void render();
	};
}


#endif