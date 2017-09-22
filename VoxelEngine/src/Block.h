#ifndef BLOCK_H
#define BLOCK_H

#include <glm\glm.hpp>
#include <Physics.h>

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
			OAK_WOOD,
			OAK_LEAVES,
			WATER,
			BEDROCK,		// Block that is indestructable. Placed at the very bottom of world
			INVALID = 255
		};
	private:
		Block();

		// Position of block in the world.
		glm::ivec3 worldCoordinate;

		// Color. Instead of vec3(12 bytes), we store value in 0~255 scale, which only needs 3bytes total
		unsigned char r;
		unsigned char g;
		unsigned char b;

		// ID
		BLOCK_ID id;

		bool init(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition);
	public:
		~Block();

		// Creates block
		static Block* create(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition);

		// Check if block is transparent. Transparent can still be a block than air.
		bool isTransparent();

		// Check if block is empty. Empty means it's air
		bool isEmpty();

		// Set color of block (0 ~ 1)
		void setColor(const glm::vec3& color);

		// Set color of block (0 ~ 255)
		void setColorRGB(const unsigned char r, const unsigned char g, const unsigned char b);
		// Set color with glm::uvec3 (0 ~ 255)
		void setColorU3(const glm::uvec3& color);

		// Get block color
		glm::vec3 getColor3();
		glm::vec4 getColor4();

		// Get world coordinate
		glm::ivec3 getWorldCoordinate();
		// Get world position
		glm::vec3 getWorldPosition();

		// Get block ID
		BLOCK_ID getBlockID();
		void setBlockID(const BLOCK_ID blockID);

		// Get AABB
		AABB getAABB();
	};
}

#endif