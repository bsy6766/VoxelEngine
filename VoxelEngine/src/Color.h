#ifndef COLOR_H
#define COLOR_H

#include <glm\glm.hpp>
#include <Utility.h>

namespace Voxel
{
	namespace Color
	{
		// Solid colors
		const glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
		const glm::vec3 BLUE = glm::vec3(0.0f, 1.0f, 0.0f);
		const glm::vec3 GREEN = glm::vec3(0.0f, 0.0f, 1.0f);
		const glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);

		// block colors
		const glm::vec3 SKYBOX = glm::vec3(150.0f / 255.0f, 210.0f / 255.0f, 1.0f);
		const glm::vec3 DAYTIME = glm::vec3(150.0f / 255.0f, 210.0f / 255.0f, 1.0f);
		const glm::vec3 NIGHTTIME = glm::vec3(15.0f / 255.0f, 21.0f / 255.0f, 25.0f / 255.0f);
		const glm::vec3 GRASS = glm::vec3(28.0f / 255.0f, 192.0f / 255.0f, 11.0f / 255.0f);
		const glm::vec3 WATER = glm::vec3(46.0f / 255.0f, 59.0f / 255.0f, 182.0f / 255.0f);
		const glm::vec3 STONE = glm::vec3(134.0f / 255.0f, 134.0f / 255.0f, 134.0f / 255.0f);
		const glm::vec3 BEDROCK = glm::vec3(33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f);

		// Random color
		static glm::vec3 getRandomColor()
		{
			return glm::vec3(Utility::Random::randomReal(0.5f, 1.0f), 
							Utility::Random::randomReal(0.5f, 1.0f), 
							Utility::Random::randomReal(0.5f, 1.0f));
		}

		static glm::ivec3 getRandomColor255()
		{
			return glm::ivec3(Utility::Random::randomInt(127, 255),
				Utility::Random::randomInt(127, 255),
				Utility::Random::randomInt(127, 255));
		}
	}
}

#endif