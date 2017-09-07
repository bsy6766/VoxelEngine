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

		// biome colors
		const glm::uvec3 OCEAN = glm::uvec3(46, 59, 182);
		const glm::uvec3 TUNDRA = glm::uvec3(170, 234, 250);
		const glm::uvec3 GRASS_DESERT = glm::uvec3(255, 188, 64);
		const glm::uvec3 DESERT = glm::uvec3(250, 148, 24);
		const glm::uvec3 TAIGA = glm::uvec3(10, 64, 57);
		const glm::uvec3 WOODS = glm::uvec3(43, 144, 57);
		const glm::uvec3 FOREST = glm::uvec3(5, 102, 33);
		const glm::uvec3 SWAMP = glm::uvec3(7, 249, 178);
		const glm::uvec3 SAVANNA = glm::uvec3(189, 178, 95);
		const glm::uvec3 SEASONAL_FOREST = glm::uvec3(71, 135, 90);
		const glm::uvec3 RAIN_FOREST = glm::uvec3(9, 111, 122);

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