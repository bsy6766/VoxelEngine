#ifndef CHUNK_SECTION_H
#define CHUNK_SECTION_H

// cpp
#include <vector>

// glm
#include <glm\glm.hpp>

// voxel
#include "Block.h"

namespace Voxel
{
	/**
	*	@class ChunkSection
	*	@brief A section of single chunk
	*
	*	Chucnk section is a part of chunk. Chunk Section manages 16 x 16 x 16 blocks.
	*/
	class ChunkSection
	{
		friend class Chunk;
		friend class ChunkMeshGenerator;
	public:
		int localBlockXYZToIndex(const int x, const int y, const int z);
		int localBlockXZToMapIndex(const int x, const int z);

		// Number of non air block size. if this is 0, chunk section can be deleted.
		unsigned int nonAirBlockSize;
	private:
		ChunkSection();

		// Position of chunk section. x and z represent the chunk's position. y represent the height of chunk section in chunk.
		glm::ivec3 position;

		// World position of chunk section. X and Z values are followed by parent chunk
		glm::vec3 worldPosition;

		// 16 x 16 x 16 blocks. TODO: Consider using Octree.
		std::vector<Block*> blocks;

		void init(const std::vector<std::vector<int>>& heightMap, const std::vector<std::vector<float>>& colorMap);
		bool init(const int x, const int y, const int z, const glm::vec3& chunkPosition, const std::vector<unsigned int>& regionMap, const std::vector<std::vector<int>>& heightMap, const std::vector<std::vector<float>>& colorMap);

		bool initEmpty(const int x, const int y, const int z, const glm::vec3& chunkPosition);
	public:
		~ChunkSection();

		// Creates chunk section.
		static ChunkSection* create(const int x, const int y, const int z, const glm::vec3& chunkPosition, const std::vector<unsigned int>& regionMap, const std::vector<std::vector<int>>& heightMap, const std::vector<std::vector<float>>& colorMap);
		// Creates chunk section. Blocks are empty.
		static ChunkSection* createEmpty(const int x, const int y, const int z, const glm::vec3& chunkPosition);

		// x,y,z must be local
		Block* getBlockAt(const int x, const int y, const int z);
		void setBlockAt(const glm::ivec3& localCoordinate, const Block::BLOCK_ID blockID, const bool overwrite = true);
		void setBlockAt(const glm::ivec3& localCoordinate, const Block::BLOCK_ID blockID, const glm::uvec3& color, const bool overwrite = true);
		void setBlockAt(const glm::ivec3& localCoordinate, const Block::BLOCK_ID blockID, const glm::vec3& color, const bool overwrite = true);
		void setBlockAt(const int x, const int y, const int z, const Block::BLOCK_ID blockID, const bool overwrite = true);
		void setBlockAt(const int x, const int y, const int z, const Block::BLOCK_ID blockID, const glm::uvec3& color, const bool overwrite = true);
		void setBlockAt(const int x, const int y, const int z, const Block::BLOCK_ID blockID, const glm::vec3& color, const bool overwrite = true);

		int getLocalTopY(const int localX, const int localZ);

		// Get world position of chunk. Center of chunk.
		glm::vec3 getWorldPosition();

		 int getTotalNonAirBlockSize();
	};
}

#endif