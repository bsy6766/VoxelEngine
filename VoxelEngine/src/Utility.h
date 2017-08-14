#ifndef UTILITY_H
#define UTILITY_H

#include <random>
#include <string>
#include <iostream>

namespace Voxel
{
	namespace Utility
	{
		class Random
		{
		private:
			static size_t seedNumber;
			static std::string seedString;

			static std::mt19937 generator;

			static bool initialized;

			static inline void generateRandomSeedString()
			{
				std::string randStr = "";
				const int len = 6;

				const char capAlphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

				for (int i = 0; i < len; i++)
				{
					randStr += capAlphabet[rand() % (sizeof(capAlphabet) - 1)];
				}

				seedString = randStr;

				std::cout << "[Utility::Random] Created new random seed: " << seedString << std::endl;
			}

			static inline void generateSeed()
			{
				generateRandomSeedString();

				seedNumber = std::hash<std::string>{}(seedString);

				generator.seed(seedNumber);
			}

		public:
			static inline std::mt19937& getGenerator()
			{
				return generator;
			}

			static inline int randomInt(int min, int max)
			{
				if (seedNumber == 0 || seedString == "")
				{
					generateSeed();
				}

				if (min > max)
				{
					std::swap(min, max);
				}

				std::uniform_int_distribution<int> dist(min, max);
				return dist(generator);
			}

			static inline float randomIntExponential()
			{
				std::exponential_distribution<double> dist(1);
				return static_cast<float>(dist(generator));
			}

			static inline int randomIntGeometric(float p)
			{
				std::geometric_distribution<> dist(p);
				return dist(generator);
			}

			static inline int randomInt100()
			{
				return randomInt(0, 100);
			}

			static inline bool randomIntRollCheck(int chance)
			{
				const int randomInt = randomInt100();
				if (randomInt <= chance)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			template<typename T>
			static inline T randomReal(T min, T max)
			{
				if (seedNumber == 0 || seedString == "")
				{
					generateSeed();
				}

				if (min > max)
				{
					std::swap(min, max);
				}

				std::uniform_real_distribution<T> dist(min, max);
				return dist(generator);
			}

			static inline float random_minus_1_1()
			{
				int rand = randomInt(0, 1);
				return rand == 0 ? 1.0f : -1.0f;
			}

			static inline void init()
			{
				if (Utility::Random::initialized != true)
				{
					seedNumber = 0;
					seedString = "";

					generateSeed();

					Utility::Random::initialized = true;
				}
			}

			static inline void setSeed(const std::string& seed)
			{
				seedString = seed;
				seedNumber = std::hash<std::string>{}(seedString);
				generator.seed(seedNumber);

				std::cout << "[Utility::Random] Created new random seed: " << seedString << std::endl;
			}

		};

	}
}

#endif