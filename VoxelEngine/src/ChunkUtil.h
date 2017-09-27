#ifndef CHUNK_UTIL_H
#define CHUNK_UTIL_H

#include <glm\glm.hpp>

namespace Voxel
{
	namespace Constant
	{
		const static int CHUNK_SECTION_HEIGHT = 32;
		const static int CHUNK_SECTION_WIDTH = 32;
		const static int CHUNK_SECTION_LENGTH = 32;
		const static unsigned int TOTAL_BLOCKS = CHUNK_SECTION_WIDTH * CHUNK_SECTION_LENGTH * CHUNK_SECTION_HEIGHT;
		const static unsigned int TOTAL_CHUNK_SECTION_PER_CHUNK = 8;
		const static int HEIGHEST_BLOCK_Y = TOTAL_CHUNK_SECTION_PER_CHUNK * CHUNK_SECTION_HEIGHT;
		const static float CHUNK_BORDER_SIZE = 32.0f;
		const static float CHUNK_BORDER_SIZE_HALF = CHUNK_BORDER_SIZE * 0.5f;
		const static float CHUNK_RANGE = CHUNK_BORDER_SIZE - 2.0f;
	}

	namespace Math
	{
		static glm::vec3 chunkXZToWorldPosition(const glm::ivec2& chunkXZ)
		{
			float x = static_cast<float>(chunkXZ.x);
			float z = static_cast<float>(chunkXZ.y);

			if (chunkXZ.x < 0) x += 1.0f;
			if (chunkXZ.y < 0) z += 1.0f;

			x *= Constant::CHUNK_BORDER_SIZE;
			z *= Constant::CHUNK_BORDER_SIZE;

			if (chunkXZ.x >= 0)
				x += Constant::CHUNK_BORDER_SIZE_HALF;
			else
				x -= Constant::CHUNK_BORDER_SIZE_HALF;

			if (chunkXZ.y >= 0)
				z += Constant::CHUNK_BORDER_SIZE_HALF;
			else
				z -= Constant::CHUNK_BORDER_SIZE_HALF;

			return glm::vec3(x, 0, z);
		}

		static unsigned int XYZToBlockIndex(const unsigned int x, const unsigned int y, const unsigned int z)
		{
			return x + (Constant::CHUNK_SECTION_WIDTH * z) + (y * Constant::CHUNK_SECTION_LENGTH * Constant::CHUNK_SECTION_WIDTH);
		}
	}

	// Hash for glm ivec2
	struct KeyFuncs
	{
		size_t operator()(const glm::ivec2& k)const
		{
			return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
		}

		bool operator()(const glm::ivec2& a, const glm::ivec2& b)const
		{
			return a.x == b.x && a.y == b.y;
		}
	};
}

#endif