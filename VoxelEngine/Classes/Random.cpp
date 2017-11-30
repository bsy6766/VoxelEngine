#include "Random.h"

using namespace Voxel;

void Voxel::Random::init(const std::string& seedString)
{
	auto seedNumber = std::hash<std::string>{}(seedString);

	globalSeed.number = seedNumber;
	globalSeed.str = seedString;

	// Rand gen 100
	randGen100.seed.number = seedNumber;
	randGen100.seed.str = seedString;
	randGen100.engine.seed(randGen100.seed.number);
	randGen100.dist = std::uniform_int_distribution<int>(0, 100);

	// Rand bool
	randGenBool.seed.number = seedNumber;
	randGenBool.seed.str = seedString;
	randGenBool.engine.seed(randGenBool.seed.number);
	randGenBool.dist = std::uniform_int_distribution<int>(0, 1);

	// Trees
	randGenTreeTrunk.seed.number = seedNumber;
	randGenTreeTrunk.seed.str = seedString;
	randGenTreeTrunk.engine.seed(randGenTreeTrunk.seed.number);
	randGenTreeTrunk.dist = std::uniform_int_distribution<int>(0, 3);

	randGenTreeTrunkHeight.seed.number = seedNumber;
	randGenTreeTrunkHeight.seed.str = seedString;
	randGenTreeTrunkHeight.engine.seed(randGenTreeTrunkHeight.seed.number);
	randGenTreeTrunkHeight.dist = std::uniform_int_distribution<int>(0, 4);
}

float Voxel::Random::getRandomFloat(float min, float max)
{
	if (min > max)
	{
		std::swap(min, max);
	}

	std::uniform_real_distribution<float> dist(min, max);
	std::mt19937 engine(globalSeed.number);
	return dist(engine);
}

int Voxel::Random::getRandGen100()
{
	return randGen100.get();
}

bool Voxel::Random::getRandGenBool()
{
	return static_cast<bool>(randGenBool.get());
}

int Voxel::Random::getRandGenTreeTrunk()
{
	return randGenTreeTrunk.get();
}

int Voxel::Random::getRandGenTreeTrunkHeight()
{
	return randGenTreeTrunkHeight.get();
}

void Voxel::Random::resetAll()
{
	resetRandGen100();
	resetRandGenBool();
}

void Voxel::Random::resetRandGen100()
{
	randGen100.engine.seed(randGen100.seed.number);
	randGen100.dist.reset();
}

void Voxel::Random::resetRandGenBool()
{
	randGenBool.engine.seed(randGenBool.seed.number);
	randGenBool.dist.reset();
}

void Voxel::Random::resetAllTreeRandGen()
{
	resetRandTreeTrunk();
	resetRandTreeTrunkHeight();
}

void Voxel::Random::resetRandTreeTrunk()
{
}

void Voxel::Random::resetRandTreeTrunkHeight()
{
}
