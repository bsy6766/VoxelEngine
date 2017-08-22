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
		enum class BLOCK_ID : unsigned char
		{
			AIR = 0,		// Default block. Transparent.
			GRASS,
			STONE,
			BEDROCK,		// Block that is indestructable. Placed at the very bottom of world
		};
	private:
		Block();

		// Local position of block in the chunk section
		//glm::ivec3 localCoordinate;
		// Position of block in the world.
		glm::ivec3 worldCoordinate;

		// World position of block in the chunk section 
		//glm::vec3 localPosition;
		// World Position of block
		//glm::vec3 worldPosition;

		// Color. Instead of vec3(12 bytes), we store value in 0~255 scale, which only needs 3bytes total
		unsigned char r;
		unsigned char g;
		unsigned char b;

		// ID
		BLOCK_ID id;

		bool init(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition);
	public:
		~Block();
		static Block* create(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition);

		// Check if block is transparent. Transparent can still be a block than air.
		bool isTransparent();
		// Check if block is empty. Empty means it's air
		bool isEmpty();

		void setColor(const glm::vec3& color);
		void setColor(const unsigned char r, const unsigned char g, const unsigned char b);
		glm::vec3 getColor();

		glm::vec3 getWorldPosition();
	};
}

#endif