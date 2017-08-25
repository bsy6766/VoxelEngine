#ifndef CHUNK_UTIL_H
#define CHUNK_UTIL_H

#include <glm\glm.hpp>

namespace Voxel
{
	namespace Constant
	{
		const static int CHUNK_SECTION_HEIGHT = 16;
		const static int CHUNK_SECTION_WIDTH = 16;
		const static int CHUNK_SECTION_LENGTH = 16;
		const static unsigned int TOTAL_BLOCKS = 4096;
		const static unsigned int TOTAL_CHUNK_SECTION_PER_CHUNK = 16;
		const static float CHUNK_BORDER_SIZE = 16.0f;
		const static int SPAWN_CHUNK_DISTANCE = 2;
		const static int REGION_WIDTH = 4;
		const static int REGION_LENGTH = 4;
	}

	namespace Direction
	{
		// East = positive X
		// West = negative X
		// South = positive Z
		// North = negative Z
		const static glm::vec3 EAST = glm::vec3(1, 0, 0);
		const static glm::vec3 WEST = glm::vec3(-1, 0, 0);
		const static glm::vec3 SOUTH = glm::vec3(0, 0, 1);
		const static glm::vec3 NORTH = glm::vec3(0, 0, -1);
	}

	namespace Math
	{
		static glm::ivec3 worldPosToBlockWorldCoordinate(const glm::vec3& position)
		{
			return glm::ivec3(position / 16.0f);
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