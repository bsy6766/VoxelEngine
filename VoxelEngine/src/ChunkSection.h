#ifndef CHUNK_SECTION_H
#define CHUNK_SECTION_H

#include <vector>
#include <glm\glm.hpp>
#include <Block.h>

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
		friend class ChunkMeshGenerator;
	public:
		int XYZToIndex(const int x, const int y, const int z);

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

		bool init(const int x, const int y, const int z, const glm::vec3& chunkPosition);
		bool initEmpty(const int x, const int y, const int z, const glm::vec3& chunkPosition);
		bool initWithHeightMap(const int x, const int y, const int z, const glm::vec3& chunkPosition, const std::vector<std::vector<int>>& heightMap);
	public:
		~ChunkSection();
		static ChunkSection* create(const int x, const int y, const int z, const glm::vec3& chunkPosition);
		static ChunkSection* createEmpty(const int x, const int y, const int z, const glm::vec3& chunkPosition);
		static ChunkSection* createWithHeightMap(const int x, const int y, const int z, const glm::vec3& chunkPosition, const std::vector<std::vector<int>>& heightMap);

		// x,y,z must be local
		Block* getBlockAt(const int x, const int y, const int z);
		void setBlockAt(const glm::ivec3& localCoordinate, const Block::BLOCK_ID blockID);
		void setBlockAt(const int x, const int y, const int z, const Block::BLOCK_ID blockID);

		// Get world position of chunk. Center of chunk.
		glm::vec3 getWorldPosition();

		 int getTotalNonAirBlockSize();
	};
}

#endif