#ifndef CHUNK_SECTION_H
#define CHUNK_SECTION_H

#include <vector>
#include <glm\glm.hpp>

namespace Voxel
{
	class Block;
	class ChunkMesh;

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
	private:
		ChunkSection();

		// Position of chunk section. x and z represent the chunk's position. y represent the height of chunk section in chunk.
		glm::ivec3 position;

		// World position of chunk section. X and Z values are followed by parent chunk
		glm::vec3 worldPosition;

		// 16 x 16 x 16 blocks. TODO: Consider using Octree.
		std::vector<Block*> blocks;

		ChunkMesh* mesh;
		// If chunk section is visible, mesh will be generated and rendered
		bool visible;

		bool init(const int x, const int y, const int z, const glm::vec3& chunkPosition);
	public:
		~ChunkSection();
		static ChunkSection* create(const int x, const int y, const int z, const glm::vec3& chunkPosition);

		// x,y,z must be local
		Block* getBlockAt(const int x, const int y, const int z);


		glm::vec3 getWorldPosition();

		std::vector<Block*>& getBlocksRef();

		void render();

		void releaseMesh();

		void setVisibility(const bool visibility);

		void setMesh(ChunkMesh* mesh);
		bool hasMesh();

		glm::ivec3 getPosition();
	};
}

#endif