#ifndef SIMPLEX_NOISE_H
#define SIMPLEX_NOISE_H

#include <random>
#include <string>
#include <array>
#include <glm\glm.hpp>
#include <memory>

namespace Voxel
{
	namespace Noise
	{
		class SimplexNoise;

		/**
		*	@class Manager
		*	@brief Simple SimplexNoise manager
		*/
		class Manager
		{
		private:
			Manager() = delete;
			~Manager() = delete;

			// Unique pointer for each noise
			static std::unique_ptr<SimplexNoise> worldNoise;	// For world gen
			static std::unique_ptr<SimplexNoise> temperatureNoise;	// For biome temperature
			static std::unique_ptr<SimplexNoise> moistureNoise;	// For biome moisture
			static std::unique_ptr<SimplexNoise> colorNoise;
		public:
			static void init(const std::string& seed);

			static SimplexNoise* getWorldNoise();
			static SimplexNoise* getTemperatureNoise();
			static SimplexNoise* getMoistureNoise();
			static SimplexNoise* getColorNoise();
		};

		/**
		*	@struct Random
		*	@Brief Simple struct that contains seed and random generator
		*/
		struct Random
		{
			size_t seedNumber;
			std::string seedString;

			std::mt19937 generator;

			Random()
				:seedNumber(0)
				, seedString("")
			{}

			inline int randomInt(int min, int max)
			{
				if (min > max)
				{
					std::swap(min, max);
				}

				std::uniform_int_distribution<int> dist(min, max);
				return dist(generator);
			}
		};

		class SimplexNoise
		{
		private:
			// Pre calculated values
			static const float F2;	//F2 = 0.5f * (sqrt(3.0f) - 1.0f)) = 0.366025403f
			static const float G2;	//G2 = (3.0f - sqrt(3.0f)) / 6.0f = 0.211324865f

			// Permutation table. This is random numbers between 0 - 255 and it's fixed. 
			// All Simplex Noise implementation uses same table.
			// Originally the size of table is 255, but doubled the size by repeating
			// sequence of numbers to avoid wrapping index at 255 for each look up
			std::array<int, 512> perm;

			// Random generator
			Random rand;

			// Gradient direction
			static const int grad3[12][3];

			// Fast floor. Faster than math.floor
			float fastFloor(float value);

			// Dot product
			float dot(const int* grad, const float x, const float y);

			void init(const std::string& seed);
		public:
			SimplexNoise(const std::string& seed);
			~SimplexNoise() = default;

			// Simplex Noise 2D
			float noise(const glm::vec2& v);
			
			// Reset perm table to default
			void reset()
			{
				perm = { 151,160,137,91,90,15,
					131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
					190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
					88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
					77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
					102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
					135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
					5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
					223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
					129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
					251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
					49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
					138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
					151,160,137,91,90,15,
					131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
					190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
					88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
					77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
					102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
					135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
					5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
					223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
					129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
					251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
					49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
					138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
			}

			// Randomize the perm table with Random class's random function
			void randomize();
		};
	}
}

#endif