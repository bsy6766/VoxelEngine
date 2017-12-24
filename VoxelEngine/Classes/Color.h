#ifndef COLOR_H
#define COLOR_H

// glm
#include <glm\glm.hpp>

// voxel
#include "Utility.h"
#include "Block.h"
#include "Random.h"

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
		
		/**
		*	@struct SkyboxColor
		*	@brief simple wrapper of 2 colors (one top and one bottom) for skybox
		*/
		struct SkyboxColor
		{
		public:
			glm::vec3 topColor;
			glm::vec3 bottomColor;

			// Constructor
			SkyboxColor(const glm::vec3& topColor, const glm::vec3& bottomColor) : topColor(topColor), bottomColor(bottomColor) {}
		};

		// Skybox colors based on time.
		const SkyboxColor SC_12_00_AM = SkyboxColor(glm::vec3(0.0f, 0.0f, 12.0f / 255.0f), glm::vec3(0.0f, 0.0f, 12.0f / 255.0f));
		const SkyboxColor SC_1_00_AM = SkyboxColor(glm::vec3(2.0f / 255.0f, 1.0f / 255.0f, 17.0f / 255.0f), glm::vec3(25.0f / 255.0f, 22.0f / 255.0f, 33.0f / 255.0f));
		const SkyboxColor SC_2_00_AM = SkyboxColor(glm::vec3(2.0f / 255.0f, 1.0f / 255.0f, 17.0f / 255.0f), glm::vec3(32.0f / 255.0f, 32.0f / 255.0f, 44.0f / 255.0f));
		const SkyboxColor SC_3_00_AM = SkyboxColor(glm::vec3(2.0f / 255.0f, 1.0f / 255.0f, 17.0f / 255.0f), glm::vec3(58.0f / 255.0f, 58.0f / 255.0f, 82.0f / 255.0f));
		const SkyboxColor SC_4_00_AM = SkyboxColor(glm::vec3(32.0f / 255.0f, 32.0f / 255.0f, 44.0f / 255.0f), glm::vec3(81.0f / 255.0f, 81.0f / 255.0f, 117.0f / 255.0f));
		const SkyboxColor SC_5_00_AM = SkyboxColor(glm::vec3(64.0f / 255.0f, 64.0f / 255.0f, 92.0f / 255.0f), glm::vec3(138.0f / 255.0f, 118.0f / 255.0f, 171.0f / 255.0f));
		const SkyboxColor SC_6_00_AM = SkyboxColor(glm::vec3(74.0f / 255.0f, 73.0f / 255.0f, 105.0f / 255.0f), glm::vec3(205.0f / 255.0f, 130.0f / 255.0f, 160.0f / 255.0f));
		const SkyboxColor SC_7_00_AM = SkyboxColor(glm::vec3(117.0f / 255.0f, 122.0f / 255.0f, 191.0f / 255.0f), glm::vec3(234.0f / 255.0f, 176.0f / 255.0f, 209.0f / 255.0f));
		const SkyboxColor SC_8_00_AM = SkyboxColor(glm::vec3(234.0f / 255.0f, 176.0f / 255.0f, 209.0f / 255.0f), glm::vec3(235.0f / 255.0f, 178.0f / 255.0f, 177.0f / 255.0f));
		const SkyboxColor SC_9_00_AM = SkyboxColor(glm::vec3(148.0f / 255.0f, 197.0f / 255.0f, 248.0f / 255.0f), glm::vec3(177.0f / 255.0f, 181.0f / 255.0f, 234.0f / 255.0f));
		const SkyboxColor SC_10_00_AM = SkyboxColor(glm::vec3(183.0f / 255.0f, 234.0f / 255.0f, 255.0f / 255.0f), glm::vec3(148.0f / 255.0f, 223.0f / 255.0f, 255.0f / 255.0f));
		const SkyboxColor SC_11_00_AM = SkyboxColor(glm::vec3(155.0f / 255.0f, 226.0f / 255.0f, 254.0f / 255.0f), glm::vec3(103.0f / 255.0f, 209.0f / 255.0f, 251.0f / 255.0f));
		const SkyboxColor SC_12_00_PM = SkyboxColor(glm::vec3(144.0f / 255.0f, 223.0f / 255.0f, 254.0f / 255.0f), glm::vec3(56.0f / 255.0f, 163.0f / 255.0f, 209.0f / 255.0f));
		const SkyboxColor SC_1_00_PM = SkyboxColor(glm::vec3(87.0f / 255.0f, 193.0f / 255.0f, 235.0f / 255.0f), glm::vec3(36.0f / 255.0f, 111.0f / 255.0f, 168.0f / 255.0f));
		const SkyboxColor SC_2_00_PM = SkyboxColor(glm::vec3(45.0f / 255.0f, 145.0f / 255.0f, 194.0f / 255.0f), glm::vec3(30.0f / 255.0f, 82.0f / 255.0f, 142.0f / 255.0f));
		const SkyboxColor SC_3_00_PM = SkyboxColor(glm::vec3(36.0f / 255.0f, 115.0f / 255.0f, 171.0f / 255.0f), glm::vec3(91.0f / 255.0f, 121.0f / 255.0f, 131.0f / 255.0f));
		const SkyboxColor SC_4_00_PM = SkyboxColor(glm::vec3(30.0f / 255.0f, 82.0f / 255.0f, 142.0f / 255.0f), glm::vec3(157.0f / 255.0f, 166.0f / 255.0f, 113.0f / 255.0f));
		const SkyboxColor SC_5_00_PM = SkyboxColor(glm::vec3(30.0f / 255.0f, 82.0f / 255.0f, 142.0f / 255.0f), glm::vec3(233.0f / 255.0f, 206.0f / 255.0f, 93.0f / 255.0f));
		const SkyboxColor SC_6_00_PM = SkyboxColor(glm::vec3(21.0f / 255.0f, 66.0f / 255.0f, 119.0f / 255.0f), glm::vec3(178.0f / 255.0f, 99.0f / 255.0f, 57.0f / 255.0f));
		const SkyboxColor SC_7_00_PM = SkyboxColor(glm::vec3(22.0f / 255.0f, 60.0f / 255.0f, 82.0f / 255.0f), glm::vec3(183.0f / 255.0f, 73.0f / 255.0f, 15.0f / 255.0f));
		const SkyboxColor SC_8_00_PM = SkyboxColor(glm::vec3(7.0f / 255.0f, 27.0f / 255.0f, 38.0f / 255.0f), glm::vec3(138.0f / 255.0f, 59.0f / 255.0f, 18.0f / 255.0f));
		const SkyboxColor SC_9_00_PM = SkyboxColor(glm::vec3(1.0f / 255.0f, 10.0f / 255.0f, 16.0f / 255.0f), glm::vec3(47.0f / 255.0f, 17.0f / 255.0f, 7.0f / 255.0f));
		const SkyboxColor SC_10_00_PM = SkyboxColor(glm::vec3(1.0f / 255.0f, 6.0f / 255.0f, 27.0f / 255.0f), glm::vec3(28.0f / 255.0f, 8.0f / 255.0f, 2.0f / 255.0f));
		const SkyboxColor SC_11_00_PM = SkyboxColor(glm::vec3(0.0f, 0.0f, 12.0f / 255.0f), glm::vec3(13.0f / 255.0f, 5.0f / 255.0f, 0.0f));
		
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
		const glm::uvec3 OAK_LEAVES = glm::uvec3(24, 99, 0);
		const glm::uvec3 BIRCH_WOOD_WHITE = glm::uvec3(214, 218, 221);
		const glm::uvec3 BIRCH_WOOD_BLACK = glm::uvec3(22, 25, 30);
		const glm::uvec3 BIRCH_LEAVES = glm::uvec3(116, 154, 51);
		const glm::uvec3 SPRUCE_WOOD = glm::uvec3(40, 25, 9);
		const glm::uvec3 SPRUCE_LEAVES = glm::uvec3(7, 66, 0);
		const glm::uvec3 PINE_WOOD = glm::uvec3(52, 33, 11);
		const glm::uvec3 PINE_LEAVES = glm::uvec3(7, 44, 3);

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

		static Voxel::Random rand;

		// Random color
		static inline glm::vec3 getRandomColor()
		{
			return glm::vec3(rand.randRangeFloat(0.4f, 0.7f), 
				rand.randRangeFloat(0.4f, 0.7f),
				rand.randRangeFloat(0.4f, 0.7f));
		}

		static inline glm::uvec3 getRandomColor255()
		{
			return glm::uvec3(rand.randRangeInt(50, 200),
				rand.randRangeInt(50, 200),
				rand.randRangeInt(50, 200));
		}

		static inline glm::vec3 colorU3TocolorV3(const glm::uvec3& color)
		{
			return glm::vec3(static_cast<float>(color.r) / 255.0f, static_cast<float>(color.g) / 255.0f, static_cast<float>(color.b) / 255.0f);
		}

		static inline glm::vec3 HSV2RGB(float h, float s, float v)
		{
			/*
			float hh, p, q, t, ff;

			int i;

			glm::vec3 out(0.0f);

			hh = h * 360.0f;
			if (hh >= 360.0f) hh = 0.0f;
			hh /= 60.0f;
			i = (int)hh;
			ff = hh - i;
			p = v * (1.0f - s);
			q = v * (1.0f - (s * ff));
			t = v * (1.0f - (s * (1.0f - ff)));

			switch (i) {
			case 0:
				out.x = v;
				out.y = t;
				out.z = p;
				break;
			case 1:
				out.x = q;
				out.y = v;
				out.z = p;
				break;
			case 2:
				out.x = p;
				out.y = v;
				out.z = t;
				break;

			case 3:
				out.x = p;
				out.y = q;
				out.z = v;
				break;
			case 4:
				out.x = t;
				out.y = p;
				out.z = v;
				break;
			case 5:
			default:
				out.x = v;
				out.y = p;
				out.z = q;
				break;
			}
			return out;
			*/

			/*
			float r = 0, g = 0, b = 0;

			if (s == 0)
			{
				r = v;
				g = v;
				b = v;
			}
			else
			{
				int i;
				float f, p, q, t;

				if (h == 360.0f)
					h = 0.0f;
				else
					h = h / 60.0f;

				i = (int)trunc(h);
				f = h - i;

				p = v * (1.0f - s);
				q = v * (1.0f - (s * f));
				t = v * (1.0f - (s * (1.0f - f)));

				switch (i)
				{
				case 0:
					r = v;
					g = t;
					b = p;
					break;

				case 1:
					r = q;
					g = v;
					b = p;
					break;

				case 2:
					r = p;
					g = v;
					b = t;
					break;

				case 3:
					r = p;
					g = q;
					b = v;
					break;

				case 4:
					r = t;
					g = p;
					b = v;
					break;

				default:
					r = v;
					g = p;
					b = q;
					break;
				}

			}

			return glm::vec3(r, g, b);
			*/


			float r, g, b;

			int i = (int)glm::floor(h * 6.0f);
			float f = h * 6.0f - i;
			float p = v * (1.0f - s);
			float q = v * (1.0f - f * s);
			float t = v * (1.0f- (1.0f - f) * s);

			switch (i % 6) {
			case 0: r = v, g = t, b = p; break;
			case 1: r = q, g = v, b = p; break;
			case 2: r = p, g = v, b = t; break;
			case 3: r = p, g = q, b = v; break;
			case 4: r = t, g = p, b = v; break;
			case 5: r = v, g = p, b = q; break;
			}

			return glm::vec3(r, g, b);
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