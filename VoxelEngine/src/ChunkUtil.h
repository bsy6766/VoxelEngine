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
		const static unsigned int TOTAL_CHUNK_SECTION_PER_CHUNK = 4;
		const static float CHUNK_BORDER_SIZE = 16.0f;
		const static int SPAWN_CHUNK_DISTANCE = 2;
	}

	namespace Math
	{
	}
}

#endif