// pch
#include "PreCompiled.h"

#include "Random.h"

Voxel::Random::Random()
	: seed(0)
	, engine(0)
{}

Voxel::Random::Random(const std::string& seedStr)
	: seed(std::hash<std::string>{}(seedStr))
{
	engine = std::mt19937(seed);
}

int Voxel::Random::randRangeInt(const int min, const int max)
{
	return (std::uniform_int_distribution<>(min, max))(engine);
}

int Voxel::Random::rand100()
{
	return (std::uniform_int_distribution<>(0, 100))(engine);
}

float Voxel::Random::randRangeFloat(const float min, const float max)
{
	return (std::uniform_real_distribution<float>(min, max))(engine);
}

float Voxel::Random::randFloatMinus1_1()
{
	return ((std::uniform_int_distribution<>(0, 100))(engine) > 50 ? 1.0f : -1.0f);
}

void Voxel::Random::setSeed(const std::string & seedStr)
{
	seed = std::hash<std::string>{}(seedStr);
	engine.seed(seed);
}

void Voxel::Random::resetEngine()
{
	engine.seed(seed);
}
