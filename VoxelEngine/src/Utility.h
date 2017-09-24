#ifndef UTILITY_H
#define UTILITY_H

#include <random>
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>		// time
#include <ctime>
#include <glm\glm.hpp>
#include <glm\gtx\string_cast.hpp>

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

			static inline float random_float_minus_1_1()
			{
				int rand = randomInt(0, 1);
				return rand == 0 ? 1.0f : -1.0f;
			}

			static inline int random_int_minus_1_1()
			{
				int rand = randomInt(0, 1);
				return rand == 0 ? 1 : -1;
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

				Utility::Random::initialized = true;

				std::cout << "[Utility::Random] Created new random seed: " << seedString << std::endl;
			}

		};

		typedef std::chrono::steady_clock::time_point tp;

		class Time
		{
		public:
			static inline std::chrono::steady_clock::time_point now()
			{
				return std::chrono::steady_clock::now();
			}

			static inline const std::string toMicroSecondString(const std::chrono::steady_clock::time_point start, const std::chrono::steady_clock::time_point end)
			{
				std::stringstream ss;
				ss << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
				return ss.str();
			}

			static inline const std::string toMilliSecondString(const std::chrono::steady_clock::time_point start, const std::chrono::steady_clock::time_point end)
			{
				std::stringstream ss;
				ss << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				return ss.str();
			}

			static inline const std::string getDate()
			{
				auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				char date[100] = { 0 };
				struct tm buf;
				localtime_s(&buf, &now);
				std::strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", &buf);
				return std::string(date);
			}
		};

		namespace Math
		{
			static float fastFloor(float value)
			{
				return static_cast<float>(value >= 0 ? static_cast<int>(value) : static_cast<int>(value) - 1);
			}

			static unsigned int findNearestPowTwo(unsigned int num)
			{
				num--;
				num |= num >> 1;
				num |= num >> 2;
				num |= num >> 4;
				num |= num >> 8;
				num |= num >> 16;
				num++;
				return num;
			}

			static float lerp(const float a, const float b, float t)
			{
				if (t > 1.0f) t = 1.0f;
				if (t < 0) t = 0;

				return (1.0f - t) * a + t * b;
			}
		}

		namespace Log 
		{
			static inline std::string vec2ToStr(const glm::vec2& val)
			{
				return glm::to_string(val);
			}

			static inline std::string vec3ToStr(const glm::vec3& val)
			{
				return glm::to_string(val);
			}
		}
	}
}

#endif