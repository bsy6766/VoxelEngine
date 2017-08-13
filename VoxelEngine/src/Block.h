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
	private:
		Block();

		// Local position of block in the chunk section
		glm::ivec3 localCoordinate;
		// Position of block in the world.
		glm::ivec3 worldCoordinate;

		// World position of block in the chunk section 
		//glm::vec3 localPosition;
		// World Position of block
		glm::vec3 position;

		// Color
		glm::vec3 color;

		// Matrix
		glm::mat4 matrix;

		bool init(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition);
	public:
		~Block();
		static Block* create(const glm::ivec3& position, const glm::ivec3& chunkSectionPosition);
	};
}

#endif