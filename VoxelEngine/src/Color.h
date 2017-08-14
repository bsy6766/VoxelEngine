#ifndef COLOR_H
#define COLOR_H

#include <glm\glm.hpp>
#include <Utility.h>

namespace Voxel
{
	namespace Color
	{
		const glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
		const glm::vec3 BLUE = glm::vec3(0.0f, 1.0f, 0.0f);
		const glm::vec3 GREEN = glm::vec3(0.0f, 0.0f, 1.0f);
		const glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);

		static glm::vec3 getRandomColor()
		{
			return glm::vec3(Utility::Random::randomReal(0.5f, 1.0f), 
							Utility::Random::randomReal(0.5f, 1.0f), 
							Utility::Random::randomReal(0.5f, 1.0f));
		}
	}
}

#endif