#ifndef RANDOM_H
#define RANDOM_H

// cpp
#include <string>
#include <random>

namespace Voxel
{
	/**
	*	@class Random
	*	@brief Uses c++ std::mt19937 to generate random number. 
	*/
	class Random
	{
	private:
		// engine seed
		size_t seed;

		// engine
		std::mt19937 engine;

	public:
		// Constructor
		Random();
		Random(const std::string& seedStr);

		// Destructor
		~Random() = default;

		/**
		*	Get random integer number in range
		*	@param min Minimum random number
		*	@param max Maximum random number
		*	@return A random int in [min, max] range.
		*/
		int randRangeInt(const int min, const int max);

		/**
		*	Get random integer number in range of [0, 100]
		*	@return A random int in range of [0, 100]
		*/
		int rand100();

		/**
		*	Get random float number in range
		*	@param min Minimum random number
		*	@param max Maximum random number
		*	@return A random falot in [min, max] range.
		*/
		float randRangeFloat(const float min, const float max);

		/**
		*	Get -1.0f or 1.0f
		*/
		float randFloatMinus1_1();

		// set seed and resets engine with new seed
		void setSeed(const std::string& seedStr);

		// Resets engine.
		void resetEngine();
	};
}

#endif