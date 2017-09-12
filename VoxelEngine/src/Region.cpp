#include "Region.h"

#include <Voronoi.h>
#include <iostream>

using namespace Voxel;

Voxel::Region::Region(Voronoi::Cell * cell)
	: cell(cell)
	, difficulty(-1)
{
}

Region::~Region()
{
}

void Voxel::Region::setDifficulty(const int difficulty)
{
	if (difficulty < 0)
	{
		return;
	}

	this->difficulty = difficulty;
}

void Voxel::Region::setDifficulty(const float totalDistance, const int pathSize, const float maxTotalDistance, const int maxPathSize, const float minBound, const float maxBound)
{
	// 50% from total distance and 50% from path size.
	// 
	
	float psf = static_cast<float>(pathSize);
	float maxPsf = static_cast<float>(maxPathSize);

	float pathSizeFactor = psf / maxPsf;

	float totalDistanceFactor = totalDistance / maxTotalDistance;

	const float pathSizeRatio = 0.65f;
	const float totalDistanceRatio = 1.0f - pathSizeRatio;

	float value = ((pathSizeFactor * pathSizeRatio) + (totalDistanceFactor * totalDistanceRatio)) * 5.0f;
	//std::cout << "Setting difficulty in Region #" << cell->getID() << " with td: " << totalDistance << ", maxTd: " << maxTotalDistance << ", ps: " << pathSize << ", maxPs: " << maxPathSize << std::endl;
	//std::cout << "Value = " << value << std::endl;
	
	difficulty = static_cast<int>(glm::round(value));
	//std::cout << "difficulty = " << difficulty << std::endl;
}

int Voxel::Region::getDifficulty()
{
	return difficulty;
}

void Voxel::Region::setAsStartingRegion()
{
	difficulty = 0;
}
