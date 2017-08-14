#ifndef CHUNK_MESH_GENERATOR_H
#define CHUNK_MESH_GENERATOR_H

#include <vector>
#include <glm\glm.hpp>

namespace Voxel
{
	// Foward
	class Chunk;
	class ChunkSection;
	class ChunkLoader;
	class ChunkMap;
	class Block;

	/**
	*	@class ChunkMeshGenerator
	*	@brief Generates mesh for single Chunk (not chunksection)
	*
	*	Single cube can have 48 verticies (including color) to 0 vertex.
	*	There are 4096 cubes in 16 x 16 x 16 (4096) in single chunk.
	*	There is total 16 chucnk sections at this moment which equals to 65536 blocks.
	*	65536 * 48 (max) verticies are 3145728, which is too much.
	*
	*	So this is why we need mesh generator. 
	*	Chunk mesh generator will only generate mesh that is visible. 
	*	Speaking of visible, it means that block that is next to transparent block like air, glass, etc.
	*	By doing this we can dramtically reduce the size of verticies and increase rendering performance
	*
	*	(Planned) However, we can once more optimize, with extra computaion.
	*	We can determine that which side of face on block that can be visible by player's position
	*	If player is facing block's front, then there is no way that player can see back side of block
	*	So we can ignore back face of block and so on.
	*	
	*	Todo: Optimize the size of number. For example, color code is between 0 ~ 255 which is 8 bit (1 byte)
	*/
	class ChunkMeshGenerator
	{
	private:
		// Returns list of block that is adjacent (up, left, right, down) from current world coordinate
		std::vector<glm::ivec3> getAdjacentBlockWorldCoordinate(const glm::ivec3& curWorldCoordiate);
	public:
		ChunkMeshGenerator() = default;
		~ChunkMeshGenerator() = default;

		void generateChunkMesh(ChunkLoader* chunkLoader, ChunkMap* chunkMap);
	};
}

#endif