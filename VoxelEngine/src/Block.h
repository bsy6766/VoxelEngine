#ifndef BLOCK_H
#define BLOCK_H

#include <glm\glm.hpp>

namespace Voxel
{
	/**
	*	@class Block
	*	@brief Contains data of the block such as position, matrix, type, etc
	*
	*	Block is a cube in the world that is placed in someplace.
	*	It doesn't rotate or scale. Instead, player(camera) moves.
	*	Some blocks might affected by gravity.
	*/
	class Block
	{
		friend class ChunkMeshGenerator;
	public:
		// block id
		enum class BLOCK_ID
		{
			AIR = 0,		// Default block. Transparent.
			GRASS,
			STONE,
			BEDROCK,		// Block that is indestructable. Placed at the very bottom of world
		};
	private:
		Block();

		// Local position of block in the chunk section
		glm::ivec3 localCoordinate;
		// Position of block in the world.
		glm::ivec3 worldCoordinate;

		// World position of block in the chunk section 
		glm::vec3 localPosition;
		// World Position of block
		glm::vec3 worldPosition;

		// Color
		glm::vec3 color;

		// ID
		BLOCK_ID id;

		bool init(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition);
	public:
		~Block();
		static Block* create(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition);

		bool isTransparent();
		bool isEmpty();

		void setColor(const glm::vec3& color);
	};
}

#endif