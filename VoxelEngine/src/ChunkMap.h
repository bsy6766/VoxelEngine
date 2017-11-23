#ifndef CHUNK_MAP_H
#define CHUNK_MAP_H

// cpp
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>

// glm
#include <glm\glm.hpp>

// gl
#include <GL\glew.h>

// Voxel
#include <Config.h>
#include <ChunkUtil.h>
#include <Block.h>
#include <Shape.h>
#include <Cube.h>
#include <Terrain.h>

namespace Voxel
{
	// forward
	class Chunk;
	class ChunkWorkManager;
	class Region;
	class Program;

	// Raycast result
	struct RayResult	
	{
	public:
		// Nullptr if ray didn't hit
		Block* block;
		// The face that ray hit
		Cube::Face face;
	};

	// shortcut with custom comparator
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

#if V_DEBUG && V_DEBUG_INIT_CHUNK_BORDER_LINE
		// Chunk debug border draw
		GLuint chunkBorderVao;
		int chunkBorderLineSize;
		glm::mat4 chunkBorderModelMat;
		bool renderChunkBorderMode;
#endif

		// render mode
		bool renderChunksMode;

		// update mode
		bool updateChunksMode;

		// Block select outline
		GLuint blockOutlineVao;
		bool renderBlockOutlineMode;

		/**
		*	Move chunk map to west (negative x)
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void moveWest(ChunkWorkManager* wm);

		/**
		*	Move chunk map to east (positive x)
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void moveEast(ChunkWorkManager* wm);

		/**
		*	Move chunk map to south (positive z)
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void moveSouth(ChunkWorkManager* wm);

		/**
		*	Move chunk map to north (negative z)
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void moveNorth(ChunkWorkManager* wm);

		/**
		*	Add row on west (negative x) chunk map
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void addRowWest(ChunkWorkManager* wm);

		/**
		*	Add row on east (positive x) chunk map
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void addRowEast(ChunkWorkManager* wm);

		/**
		*	Add row on south (positive z) chunk map
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void addColSouth(ChunkWorkManager* wm);

		/**
		*	Add row on north (negative z) chunk map
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void addColNorth(ChunkWorkManager* wm);

		/**
		*	Remove row on west (negative x) chunk map
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void removeRowWest(ChunkWorkManager* wm);

		/**
		*	Remove row on east (positive x) chunk map
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void removeRowEast(ChunkWorkManager* wm);

		/**
		*	Remove row on south (positive z) chunk map
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void removeColSouth(ChunkWorkManager* wm);

		/**
		*	Remove row on north (negative z) chunk map
		*	@param wm ChunkWorkManager pointer to add work.
		*/
		void removeColNorth(ChunkWorkManager* wm);
	public:
		// constructor
		ChunkMap();
		// destructor
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

		/**
		*	Get chunk at coordinate x and z
		*	@return Shared pointer of chunk. nullptr if chunk doesn't exsits
		*/
		std::shared_ptr<Chunk> getChunkAtXZ(int x, int z);

		/**
		*	Get chunk at coordinate x and z. Calls getChunkAtXZ(int, int).
		*	@return Shared pointer of chunk. nullptr if chunk doesn't exsits
		*/
		std::shared_ptr<Chunk> getChunkAtXZ(const glm::ivec2& chunkXZ);

		/**
		*	Get list of nearby chunk from give chunk coordinate.
		*	Doesn't incldues itself.
		*	@return 2D vector of shared Chunk pointers. nullptr if chunk doesn't exists
		*/
		std::vector<std::vector<std::shared_ptr<Chunk>>> getNearByChunks(const glm::ivec2& chunkXZ);
		
		/**
		*	Generates empty chunk at coordinate
		*	@param x Chunk coordinate in x axis
		*	@param z Chunk coordinate in z axis
		*/
		void generateEmptyChunk(const int x, const int z);

		// Get size of map
		unsigned int getSize();

		/**
		*	Converts block world coordinate to block local coordinate and chunk section coordinate.
		*	@param blockWorldCoordinate Block's world coordinate to convert.
		*	@param blockLocalCoordinate A ref of block's local coordinate to get.
		*	@param chunkSectionCoordinate A ref of chunk section coordinate to get.
		*/
		void blockWorldCoordinateToLocalAndChunkSectionCoordinate(const glm::ivec3& blockWorldCoordinate, glm::ivec3& blockLocalCoordinate, glm::ivec3& chunkSectionCoordinate);

		/**
		*	Converts player position to block world coordinate
		*	@param playerPosition Player's position
		*	@return Block world coordinate that player is standing.
		*/
		glm::ivec3 playerPosToBlockWorldCoordinate(const glm::vec3& playerPosition);

		/**
		*	Query and get list of chunk coordinates nearby the block.
		*	Includes the chunk where block exists.
		*	@param blockLocalPos Block's local position
		*	@param blockChunkPos Block's chunk position
		*	@return List of chunk coordinates near by Block.
		*/
		std::vector<glm::ivec2> getChunksNearByBlock(const glm::ivec3& blockLocalPos, const glm::ivec3& blockChunkPos);

		// Get block in world position. Returns nullptr if chunk, chunksection or block doesn't exsits.
		// boo valid tells wether block exists or not. It will be true if block exists. However, return value can still be nullptr if it's air block. False if chunk or chunk section doesn't exists = no block.
		/**
		*	Get block at world coordinate
		*	@param x Coordinate in x axis
		*	@param y Coordinate in y axis
		*	@param z Coordinate in z axis
		*	@return Block pointer. Can be nullptr of block is air block. Also can be nullptr if block, chunk section or chunk doesn't exists.
		*/
		Block* getBlockAtWorldXYZ(int x, int y, int z);

		/**
		*	Get block at world position.
		*	Converts worldPosition to world coordinate and calsl getBlockATWorldXYZ(int, int, int)
		*	@see getBlockAtWorldXYZ(int, int, int)
		*	@param worldPosition World position of block to get.
		*	@return Block pointer. Can be nullptr of block is air block. Also can be nullptr if block, chunk section or chunk doesn't exists.
		*/
		Block* getBlockAtWorldXYZ(const glm::vec3& worldPosition);
		
		// Check if block is opaque.
		enum class BLOCK_QUERY_RESULT : int
		{
			NONE = -1,
			EXIST_TRANSPARENT = 0,		// block exists and it's transparent
			EXIST_OPAQUE,				// block exists and it's opaque
			NO_CHUNK_SECTION,			// chunk section doesn't exists
			NO_CHUNK,					// chunk doesn't exists
			INACTIVE_CHUNK,				// chunk is inactive.
		};

		typedef BLOCK_QUERY_RESULT BQR;

		/**
		*	Checks if block at world coordinate is opaque.
		*	@param x Coordinate in x axis
		*	@param y Coordinate in y axis
		*	@param z Coordinate in z axis
		*	@return Result of query. @see BLOCK_QUERY_RESULT for details.
		*/
		BQR isBlockAtWorldXYZOpaque(const int x, const int y, const int z);

		/**
		*	Places a block at world coordinate
		*	@param blockWorldCoordinate Block's world coordinate to place.
		*	@param blockID Block's ID to place. If it's air, block becomes nullptr.
		*	@param wm ChunkWorkManager pointer to refresh chunk
		*	@param overwrite true by default. If true, overwrites existing block. Else, do nothing.
		*/
		void placeBlockAt(const glm::ivec3& blockWorldCoordinate, const Block::BLOCK_ID blockID, ChunkWorkManager* wm, const bool overwrite = true);

		/**
		*	Places a block at world coordinate
		*	@param blockWorldCoordinate Block's world coordinate to place.
		*	@param blockID Block's ID to place. If it's air, block becomes nullptr.
		*	@param color Block's color in 0 ~ 255 value.
		*	@param wm ChunkWorkManager pointer to refresh chunk
		*	@param overwrite true by default. If true, overwrites existing block. Else, do nothing.
		*/
		void placeBlockAt(const glm::ivec3& blockWorldCoordinate, const Block::BLOCK_ID blockID, const glm::uvec3& color, ChunkWorkManager* wm, const bool overwrite = true);

		/**
		*	Places a block at world coordinate
		*	@param blockWorldCoordinate Block's world coordinate to place.
		*	@param blockID Block's ID to place. If it's air, block becomes nullptr.
		*	@param color Block's color in 0 ~ 1 value
		*	@param wm ChunkWorkManager pointer to refresh chunk
		*	@param overwrite true by default. If true, overwrites existing block. Else, do nothing.
		*/
		void placeBlockAt(const glm::ivec3& blockWorldCoordinate, const Block::BLOCK_ID blockID, const glm::vec3& color, ChunkWorkManager* wm, const bool overwrite = true);

		/**
		*	Places a block from face of block
		*	@param blockWorldCoordinate Block's world coordinate.
		*	@param blockID Block's ID to place. If it's air, block becomes nullptr.
		*	@param faceDir Direction of block's face. New block's destination depends on this.
		*	@param wm ChunkWorkManager pointer to refresh chunk
		*/
		void placeBlockFromFace(const glm::ivec3& blockWorldCoordinate, const Block::BLOCK_ID blockID, const Cube::Face& faceDir, ChunkWorkManager* workManager);

		/**
		*	Removes a block at world coordinate. Removed block becomes nullptr, which also equals to air block.
		*	@param blockWorldCoordinate Block's world coordinate to remove.
		*	@param wm ChunkWorkManager pointer to refresh chunk
		*/
		void removeBlockAt(const glm::ivec3& blockWorldCoordinate, ChunkWorkManager* workManager);
		
		/**
		*	Raycasts block from player's eye position.
		*	This doesn't actually do ray intersection check with blocks. 
		*	It iterates and progress ray from player's eye position by small amount, step by step. Checks if ray is in oqaque block.
		*	@param playerEyePosition Player's eye position in world.
		*	@param playerDirection Player's direction.
		*	@param playerRange Player's raycast range.
		*	@return A RayResult that contains block pointer and face of block that is raycasted.
		*/
		RayResult raycastBlock(const glm::vec3& playerEyePosition, const glm::vec3& playerDirection, const float playerRange);

		/**
		*	Raycasts for camera collision.
		*	@param rayStart Player's eye position in world
		*	@param rayEnd Maximum camera position from player's eye position based on player's direction and current camera range
		*	@param maxCameraRange Maximum range of that camera can extend.
		*	@return Minimum distance from camera's position and player's eye.
		*/
		float raycastCamera(const glm::vec3& rayStart, const glm::vec3& rayEnd, const float maxCameraRange);
		
		/**
		*	Release chunk.
		*	This releases mesh of chunk and removes from chunk map
		*	@param Chunk coordinate to release.
		*/
		void releaseChunk(const glm::ivec2& coordinate);

		// Get number of active chunks
		int getActiveChunksCount();

		/**
		*	Check if chunk is on edge. Edge means end of render distance. Doesn't incldues extra rows and cols in active chunk.
		*	@param chunkXZ Chunk coordinate to check.
		*	@retrun true if chunk is on edge of active chunks. Else, false.
		*/
		bool isChunkOnEdge(const glm::ivec2& chunkXZ);

		/**
		*	Get chunk position in XZ coordinate that player is at
		*	@param playerPosition Position of player in world.
		*	@return Chunk coordinate in XZ axis.
		*/
		glm::ivec2 getPlayerChunkPos(const glm::vec3& playerPosition);

		/**
		*	Check if player is near by chunk coordinate. 
		*	Used to update current chunk position in chunk map
		*	@return true if player is close enough to chunk
		*/
		bool isPlayerNearByChunk(const glm::vec3& playerPosition, const glm::ivec2& chunkXZ);

		/**
		*	Updates current chunk pos.
		*	Checks if player is close enough to another chunk. If so, updates current chunk cordinate
		*	@return true if player actually moved far enough to new chunk. Else, false
		*/
		bool updateCurrentChunkPos(const glm::vec3& playerPosition);

		/**
		*	Updates chunk map. 
		*	Updates active chunk lists based on chunk distance that player moved. 
		*	@param chunkDist Distance in chunk coordinate that player moved 
		*	@param workManager ChunkWorkManager pointer to request work.
		*/
		void update(const glm::ivec2& chunkDist, ChunkWorkManager* workManager);
		
		/**
		*	Updates chunk border debug line model matrix. Uses chunk's model matrix
		*/
		void updateChunkBorderDebugLineModelMat();
		
		/**
		*	Find visible chunks based on render distance
		*	@param renderDistance Number of chunks that are rendered from player's position.
		*	@return Number of visible chunks.
		*/
		int findVisibleChunk(const int renderDistance);

		/**
		*	Find and get visible chunks coordinate.
		*	@param visibleChunks A ref list of visible chunks to get. Empties if there is no visible chunk.
		*	@return Number of visible chunks.
		*/
		int findVisibleChunk(std::vector<glm::ivec2>& visibleChunks);

		/**
		*	Find and get visible chunks coordinate.
		*	@param visibleChunks A ref set of visible chunks to get. Empties if there is no visible chunk.
		*	@return Number of visible chunks.
		*/
		int findVisibleChunk(std::unordered_set<glm::ivec2, KeyFuncs, KeyFuncs>& visibleChunks);

#if V_DEBUG && V_DEBUG_INIT_CHUNK_BORDER_LINE
		/**
		*	Set mode for chunk border debug line.
		*	@param mode A bool mode to set.
		*/
		void setRenderChunkBorderMode(const bool mode);
#endif

		/**
		*	Set mode for chunk rendering. Renders chunk if mode is true. Else, doesn't renders.
		*	@param mode A bool mode to set.
		*/
		void setRenderChunksMode(const bool mode);

		/**
		*	Set mode for block outline. Renders block outline if mode is true. Else, doesn't renders.
		*	@param mode A bool mode to set.
		*/
		void setRenderBlockOutlineMode(const bool mode);

		/**
		*	Set mode for chunk map update. Updates chunk map if it's true. Else, doesn't updates chunk map.
		*	@param mode A bool mode to set.
		*/
		void setUpdateChunkMapMode(const bool mode);

		// Set terrain type for region
		void setRegionTerrainType(const unsigned int regionID, const Terrain& terrainType);
		std::unordered_map<unsigned int, Terrain>& getRegionTerrainsMap();
			
		// Get current chunk xz pos
		glm::ivec2 getCurrentChunkXZ();
		
		/**
		*	Queries collidable blocks near player in x and z axis primarily. This doesn't queries blocks underneath or above.
		*	Range of query is 1 block wide and long.
		*	@param playerPosition Player's position to query.
		*	@param collidableBlocks A ref list of block pointers that are collidable.
		*/
		void queryNearByCollidableBlocksInXZ(const glm::vec3& playerPosition, std::vector<Block*>& collidableBlocks);

		/**
		*	Queries collidable blocks that are under player's position. 
		*	@param playerPosition Player's position to query.
		*	@param collidableBlocks A ref list of block pointers that are collidable.
		*/
		void queryBottomCollidableBlocksInY(const glm::vec3& playerPosition, std::vector<Block*>& collidableBlocks);

		/**
		*	Queries collidable blocks that are above player's position.
		*	@param playerPosition Player's position to query.
		*	@param collidableBlocks A ref list of block pointers that are collidable.
		*/
		void queryTopCollidableBlocksInY(const glm::vec3& playerPosition, std::vector<Block*>& collidableBlocks);

		/**
		*	Queries collidable blocks nearby the given position, range of 1.
		*	@param position Position to query.
		*	@param collidableBlocks A ref list of block pointers that are collidable.
		*/
		void queryNearByBlocks(const glm::vec3& position, std::vector<Block*>& collidableBlocks);

		// Get top y at 
		int getTopYAt(const glm::vec2& position);

		// render chunks
		void render(const glm::vec3& playerPosition);

		// render chunk border debug
		void renderChunkBorder(Program* program);
		// Debug
		void renderCameraChunkBorder(Program* program, const glm::vec3& cameraPosition);

		// render block outline
		void renderBlockOutline(Program* lineProgram, const glm::vec3& blockPosition);

		// debug print
		void printCurrentChunk();
		void printChunk(const glm::ivec2& chunkXZ);

		// debug print
		void printChunkMap();
		void printActiveChunks();
	};
}


#endif