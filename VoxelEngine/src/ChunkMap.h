#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

#include <glm\glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <ChunkUtil.h>
#include <Block.h>
#include <Geometry.h>
#include <mutex>
#include <Cube.h>
#include <Terrain.h>
#include <memory>
#include <GL\glew.h>

namespace Voxel
{
	// forward
	class Chunk;
	class ChunkWorkManager;
	class Region;
	class Program;

	// Raycast result
	struct RayResult	{
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

		// Cache the terrain type for each region. This is used when work manager generates chunk's hight map. chunk has region map and need terrain type for each region value in region map.
		std::unordered_map<unsigned int, Terrain> regionTerrainsMap;

		// Chunk map's min and max XZ coordinate
		glm::ivec2 minXZ;
		glm::ivec2 maxXZ;

		// Chunk debug border draw
		GLuint chunkBorderVao;
		int chunkBorderLineSize;
		glm::mat4 chunkBorderModelMat;

		// render mode
		bool renderChunksMode;
		bool renderChunkBorderMode;
		bool renderBlockOutlineMode;

		// update mode
		bool updateChunksMode;

		// Block select outline
		GLuint blockOutlineVao;

		// Move calls when player moves to new chunk
		void moveWest(ChunkWorkManager* wm);
		void moveEast(ChunkWorkManager* wm);
		void moveSouth(ChunkWorkManager* wm);
		void moveNorth(ChunkWorkManager* wm);

		// map modification
		// add
		void addRowWest(ChunkWorkManager* wm);
		void addRowEast(ChunkWorkManager* wm);
		void addColSouth(ChunkWorkManager* wm);
		void addColNorth(ChunkWorkManager* wm);
		// remove
		void removeRowWest(ChunkWorkManager* wm);
		void removeRowEast(ChunkWorkManager* wm);
		void removeColSouth(ChunkWorkManager* wm);
		void removeColNorth(ChunkWorkManager* wm);
	public:
		ChunkMap();
		~ChunkMap();

		// Initialize map data near by player based on player's last position and render distance
		std::vector<glm::vec2> initChunkNearPlayer(const glm::vec3& playerPosition, const int renderDistance);

		// Initialize active chunks 
		void initActiveChunks();

		// Initialzie chunk border lines
		void initChunkBorderDebug(Program* program);

		// Initialize block outline
		void initBlockOutline(Program* program);

		// Clears all the chunk in the map
		void clear();
		// Clears all mesh in the chunk
		void clearAllMeshes();
		// Rebuilds all mesh
		void rebuildAllMeshes(ChunkWorkManager* wm);

		// Check if has chunk in x and z coordinate
		bool hasChunkAtXZ(int x, int z);

		// Get chunk in x and z coordinate
		//Chunk* getChunkAtXZ(int x, int z);
		std::shared_ptr<Chunk> getChunkAtXZ(int x, int z);

		std::vector<std::vector<std::shared_ptr<Chunk>>> getNearByChunks(const glm::ivec2& chunkXZ);
		
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
		Block* getBlockAtWorldXYZ(const glm::vec3& worldPosition);
		
		// Check if block is opaque.
		// Retruns 0 if block exists and transparent. 
		// Returns 1 if block exists and opaque
		// Retruns 2 if chunk section doesn't exists
		// Retruns 3 if chunk doesn't exsits.
		// Returns 4 if chunk is inactive
		int isBlockAtWorldXYZOpaque(const int x, const int y, const int z);

		// Place block at block world coordinate.
		void placeBlockAt(const glm::ivec3& blockWorldCoordinate, const Block::BLOCK_ID blockID, ChunkWorkManager* wm, const bool overwrite = true, const bool byPlayer = false);
		void placeBlockAt(const glm::ivec3& blockWorldCoordinate, const Block::BLOCK_ID blockID, const glm::uvec3& color, ChunkWorkManager* wm, const bool overwrite = true, const bool byPlayer = false);
		void placeBlockAt(const glm::ivec3& blockWorldCoordinate, const Block::BLOCK_ID blockID, const glm::vec3& color, ChunkWorkManager* wm, const bool overwrite = true, const bool byPlayer = false);
		// place block at face direction. BlockPos is world coordinate
		void placeBlockFromFace(const glm::ivec3& blockWorldCoordinate, const Block::BLOCK_ID blockID, const Cube::Face& faceDir, ChunkWorkManager* workManager);
		void removeBlockAt(const glm::ivec3& blockWorldCoordinate, ChunkWorkManager* workManager);
		
		// From rayStart to rayEnd, visit all blocks
		RayResult raycastBlock(const glm::vec3& playerEyePosition, const glm::vec3& playerDirection, const float playerRange);
		float raycastCamera(const glm::vec3& rayStart, const glm::vec3& rayEnd, const float cameraRange);

		// Check which face of block(cube) did ray hit
		Cube::Face raycastFace(const glm::vec3& rayStart, const glm::vec3& rayEnd, const Geometry::AABB& blockAABB);
		int raycastTriangle(const glm::vec3& rayStart, const glm::vec3& rayEnd, const Geometry::Triangle& tri, glm::vec3& intersectingPoint);
		float raycastIntersectingDistance(const glm::vec3& rayStart, const glm::vec3& rayEnd, const Geometry::AABB& blockAABB);

		// Release and delete chunk 
		void releaseChunk(const glm::ivec2& coordinate);

		// Get number of active chunks
		int getActiveChunksCount();

		// Check if chunk is on the edge of grid.
		bool isChunkOnEdge(const glm::ivec2& chunkXZ);

		// Update chunk map
		glm::ivec2 checkPlayerChunkPos(const glm::vec3& playerPosition);
		bool update(const glm::ivec2& newChunkXZ, ChunkWorkManager* workManager, const double time);
		
		// find visible chunks. retrusn the number of chunks that is visible
		int findVisibleChunk();
		int findVisibleChunk(std::vector<glm::ivec2>& visibleChunks);
		int findVisibleChunk(std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs>& visibleChunks);

		// mode setter
		void setRenderChunkBorderMode(const bool mode);
		void setRenderChunksMode(const bool mode);
		void setRenderBlockOutlineMode(const bool mode);
		void setUpdateChunkMapMode(const bool mode);

		// Set terrain type for region
		void setRegionTerrainType(const unsigned int regionID, const Terrain& terrainType);
		std::unordered_map<unsigned int, Terrain>& getRegionTerrainsMap();
			
		// Get current chunk xz pos
		glm::ivec2 getCurrentChunkXZ();

		void getCollidableBlockNearPlayer(const glm::vec3& playerPosition, std::vector<Block*>& collidableBlocks);
		void queryNearByCollidableBlocksInXZ(const glm::vec3& playerPosition, std::vector<Block*>& collidableBlocks);
		void queryBottomCollidableBlocksInY(const glm::vec3& playerPosition, std::vector<Block*>& collidableBlocks);
		void queryTopCollidableBlocksInY(const glm::vec3& playerPosition, std::vector<Block*>& collidableBlocks);
		void queryNearByBlocks(const glm::vec3& position, std::vector<Block*>& collidableBlocks);

		// Get top y at 
		int getTopYAt(const glm::vec2& position);

		// render chunks
		void render();

		// render chunk border debug
		void renderChunkBorder(Program* program);

		// render block outline
		void renderBlockOutline(Program* lineProgram, const glm::vec3& blockPosition);

		// debug print
		void printCurrentChunk();

		// debug print
		void printChunkMap();
		void printActiveChunks();
	};
}


#endif