#ifndef RANDOM_H
#define RANDOM_H

// cpp
#include <random>
#include <string>

namespace Voxel
{
	/**
	*	@class Random
	*	@brief Random singleton class. Manages all the random in the game
	*/
	class Random
	{
	private:
		struct Seed
		{
			size_t number;
			std::string str;
		};

		template<typename T>
		struct Generator
		{
			Seed seed;
			std::mt19937 engine;
			std::uniform_int_distribution<T> dist;

			T get()
			{
				return dist(engine);
			}
		};
	private:
		Random() = default;
		~Random() = default;

		// Global seed
		Seed globalSeed;

		// Rand100. Generates number 0 ~ 100 in uniform distribution
		Generator<int> randGen100;

		// Rand bool. Generate random true or false
		Generator<int> randGenBool;

		// For tree types
		Generator<int> randGenTreeTrunk;
		Generator<int> randGenTreeTrunkHeight;
	public:
		static Random& getInstance()
		{
			// Since it's a static variable, if the class has already been created,
			// It won't be created again.
			// And it **is** thread-safe in C++11.

			static Random instance;

			// Return a reference to our instance.
			return instance;
		}

		// delete copy and move constructors and assign operators
		Random(Random const&) = delete;             // Copy construct
		Random(Random&&) = delete;                  // Move construct
		Random& operator=(Random const&) = delete;  // Copy assign
		Random& operator=(Random &&) = delete;      // Move assign

		// Initialize instance. Call this once at the start of app.
		void init(const std::string& seedString);

		// Generate random float. This does not generate same sequence unliek other.
		float getRandomFloat(float min, float max);

		// Generate random 100
		int getRandGen100();

		// Generate random bool
		bool getRandGenBool();

		// Generate random tree trunk type
		int getRandGenTreeTrunk();

		// Generate random tree trunk height
		int getRandGenTreeTrunkHeight();

		// Reset all randoms
		void resetAll();

		// Reset random 100
		void resetRandGen100();

		// Reset random bool
		void resetRandGenBool();

		// Reset all tree randoms
		void resetAllTreeRandGen();

		// Reset random tree trunk 
		void resetRandTreeTrunk();

		// Reset random tree trunk height
		void resetRandTreeTrunkHeight();
	};
}

#endif