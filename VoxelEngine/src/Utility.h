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
		/**
		*	@class Random
		*	@brief General random generator. All functions and members are static.
		*/
		class Random
		{
		public:
			struct Seed
			{
				size_t number;
				std::string str;
			};

			struct Generator
			{
				Seed seed;
				std::mt19937 engine;
				std::uniform_int_distribution<int> dist;

				int get()
				{
					return dist(engine);
				}
			};
		private:
			// Seed in humber. Hashed from seed string
			static size_t seedNumber;

			// Seed string.
			static std::string seedString;

			// Generator. Uses mt19937.
#ifdef _WIN64
			static std::mt19937_64 generator;
#else
			static std::mt19937 generator;
#endif

			// True if initailized (seed generated)
			static bool initialized;

			// Distribution. Need to store all distribution to generate same number of sequence everytime based on seed.
			static std::uniform_int_distribution<int> rand100Dist;
			static std::mt19937 rand100Generator;

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

				std::cout << "[Utility::Random] Created new random seed: " << seedString << "\n";
			}

			static inline void generateSeed()
			{
				generateRandomSeedString();

				seedNumber = std::hash<std::string>{}(seedString);

				generator.seed(seedNumber);
			}

		public:
			static inline void resetGenerator()
			{
				rand100Generator.seed(seedNumber);
				rand100Dist.reset();
			}

			static inline int randomInt(int min, int max)
			{
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
				return rand100Dist(rand100Generator);
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
				rand100Generator.seed(seedNumber);

				Utility::Random::initialized = true;

				std::cout << "[Utility::Random] Created new random seed: " << seedString << ", seed number: " << seedNumber << "\n";
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

			static inline const std::string toMicroSecondString(const std::chrono::steady_clock::time_point start, const std::chrono::steady_clock::time_point end, const bool addUnit = true)
			{
				std::stringstream ss;
				ss << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
				return ss.str() + "¥ìs";
			}

			static inline const std::string toMilliSecondString(const std::chrono::steady_clock::time_point start, const std::chrono::steady_clock::time_point end, const bool addUnit = true)
			{
				std::stringstream ss;
				ss << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				return ss.str() + "ms";
			}

			static inline const std::string toNanoSecondSTring(const std::chrono::steady_clock::time_point start, const std::chrono::steady_clock::time_point end, const bool addUnit = true)
			{
				std::stringstream ss;
				ss << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
				return ss.str() + "ns";
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

		namespace String
		{
			/**
			*	Finds and removes file extention from file name.
			*	It will look for last occuring '.' in the file name and removes all characters to the end in the string.
			*/
			static inline std::string removeFileExtFromFileName(const std::string& fileName)
			{
				size_t lastIndex = fileName.find_last_of(".");
				if (lastIndex == std::string::npos)
				{
					return fileName;
				}
				else
				{
					return fileName.substr(0, lastIndex);
				}
			}

			static inline void fileNameToNameAndExt(const std::string& fileName, std::string& name, std::string& ext = std::string(".png"))
			{
				size_t lastindex = fileName.find_last_of(".");
				
				if (lastindex == std::string::npos)
				{
					name = fileName;
				}
				else
				{
					name = fileName.substr(0, lastindex);
					ext = fileName.substr(lastindex);
				}
			}
		}

		namespace Polygon
		{
			static bool isPointInPolygon(const std::vector<glm::vec2>& polygon, const glm::vec2& point)
			{
				int nvert = static_cast<int>(polygon.size());

				int i, j, c = 0;

				for (i = 0, j = nvert - 1; i < nvert; j = i++)
				{
					if (((polygon.at(i).y > point.y) != (polygon.at(j).y > point.y)) && (point.x < (polygon.at(j).x - polygon.at(i).x) * (point.y - polygon.at(i).y) / (polygon.at(j).y - polygon.at(i).y) + polygon.at(i).x))
						c = !c;
				}

				if (c)
				{
					return true;
				}
				else
				{
					for (int i = 0; i < nvert - 1; i++)
					{
						if (glm::distance(polygon.at(i), point) + glm::distance(polygon.at(i + 1), point) == glm::distance(polygon.at(i), polygon.at(i + 1)))
						{
							return true;
						}
					}

					if (glm::distance(polygon.front(), point) + glm::distance(polygon.back(), point) == glm::distance(polygon.front(), polygon.back()))
					{
						return true;
					}

				}

				return false;
			}
		}
	}
}

#endif