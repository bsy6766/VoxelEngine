#ifndef COLOR_H
#define COLOR_H

#include <glm\glm.hpp>
#include <Utility.h>
#include <Block.h>

namespace Voxel
{
	namespace Color
	{
		// Solid colors
		const glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
		const glm::vec3 BLUE = glm::vec3(0.0f, 1.0f, 0.0f);
		const glm::vec3 GREEN = glm::vec3(0.0f, 0.0f, 1.0f);
		const glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);

		// skybox
		const glm::vec3 SKYBOX = glm::vec3(150.0f / 255.0f, 210.0f / 255.0f, 1.0f);
		const glm::vec3 DAYTIME = glm::vec3(150.0f / 255.0f, 210.0f / 255.0f, 1.0f);
		const glm::vec3 NIGHTTIME = glm::vec3(15.0f / 255.0f, 21.0f / 255.0f, 25.0f / 255.0f);

		// block colors
		//const glm::vec3 GRASS = glm::vec3(28.0f / 255.0f, 192.0f / 255.0f, 11.0f / 255.0f);
		//const glm::vec3 GRASS = glm::vec3(54.0f / 255.0f, 185.0f / 255.0f, 41.0f / 255.0f);
		//const glm::vec3 WATER = glm::vec3(46.0f / 255.0f, 59.0f / 255.0f, 182.0f / 255.0f);
		//const glm::vec3 STONE = glm::vec3(134.0f / 255.0f, 134.0f / 255.0f, 134.0f / 255.0f);
		//const glm::vec3 BEDROCK = glm::vec3(33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f);

		// Block colors
		const glm::uvec3 GRASS = glm::uvec3(54, 185, 41);
		const glm::uvec3 GRASS_MIX = glm::uvec3(185, 210, 57);
		const glm::uvec3 STONE = glm::uvec3(134, 134, 134);
		const glm::uvec3 OAK_WOOD = glm::uvec3(76, 50, 18);
		const glm::uvec3 OAK_LEAVES = glm::uvec3(0, 120, 42);

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

		// Difficulty color
		const glm::uvec3 DIFFICULTY_0 = glm::uvec3(139, 151, 255);
		const glm::uvec3 DIFFICULTY_1 = glm::uvec3(43, 64, 254);
		const glm::uvec3 DIFFICULTY_2 = glm::uvec3(52, 255, 215);
		const glm::uvec3 DIFFICULTY_3 = glm::uvec3(52, 255, 103);
		const glm::uvec3 DIFFICULTY_4 = glm::uvec3(242, 255, 54);
		const glm::uvec3 DIFFICULTY_5 = glm::uvec3(255, 177, 52);
		const glm::uvec3 DIFFICULTY_6 = glm::uvec3(255, 52, 52);
		const glm::uvec3 DIFFICULTY_7 = glm::uvec3(184, 0, 0);

		// Random color
		static inline glm::vec3 getRandomColor()
		{
			return glm::vec3(Utility::Random::randomReal(0.5f, 1.0f), 
							Utility::Random::randomReal(0.5f, 1.0f), 
							Utility::Random::randomReal(0.5f, 1.0f));
		}

		static inline glm::uvec3 getRandomColor255()
		{
			return glm::uvec3(Utility::Random::randomInt(50, 200),
				Utility::Random::randomInt(50, 200),
				Utility::Random::randomInt(50, 200));
		}

		static inline glm::vec3 colorU3TocolorV3(const glm::uvec3& color)
		{
			return glm::vec3(static_cast<float>(color.r) / 255.0f, static_cast<float>(color.g) / 255.0f, static_cast<float>(color.b) / 255.0f);
		}

		static inline glm::uvec3 getColorU3FromBlockID(const Block::BLOCK_ID blockID)
		{
			glm::uvec3 color;

			switch (blockID)
			{
			case Block::BLOCK_ID::GRASS:
				color = Color::GRASS;
				break;
			case Block::BLOCK_ID::STONE:
				color = Color::STONE;
				break;
			case Block::BLOCK_ID::OAK_WOOD:
				color = Color::OAK_WOOD;
				break;
			case Block::BLOCK_ID::OAK_LEAVES:
				color = Color::OAK_LEAVES;
				break;
			default:
				break;
			}

			return color;
		}

		static inline glm::uvec3 getDifficultyColor(const int difficulty)
		{
			switch (difficulty)
			{
			case 0:
				return DIFFICULTY_0;
				break;
			case 1:
				return DIFFICULTY_1;
				break;
			case 2:
				return DIFFICULTY_2;
				break;
			case 3:
				return DIFFICULTY_3;
				break;
			case 4:
				return DIFFICULTY_4;
				break;
			case 5:
				return DIFFICULTY_5;
				break;
			case 6:
				return DIFFICULTY_6;
				break;
			case 7:
				return DIFFICULTY_7;
				break;
			default:
				return glm::uvec3(255);
				break;
			}
		}
	}
}

#endif